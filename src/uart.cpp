#include "uart.h"

Uart *uart_handler = 0;

Uart::Uart()
    : _handle{nullptr} {
}

void Uart::begin(uint32_t baudrate) {
    _config = {
        .ui32BaudRate = baudrate,
        .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 | AM_HAL_UART_RX_FIFO_1_2),
        .pui8TxBuffer = _tx_buff,
        .ui32TxBufferSize = sizeof(_tx_buff),
        .pui8RxBuffer = nullptr,  // Handle reading FIFO in ISR
        .ui32RxBufferSize = 0,
    };

    // Initialize Uart
    if (_handle == nullptr) {
        am_hal_uart_initialize(0, &_handle);
    }

    am_hal_uart_power_control(_handle, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(_handle, &_config);

    // Enable UART pins
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    // Enable RX and TX interrupts (TXCMP needed to signal when TX becomes idle)
    am_hal_uart_interrupt_enable(_handle, (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT | AM_HAL_UART_INT_TX | AM_HAL_UART_INT_TXCMP));

    // Serviec interrupts to determine idle state
    am_hal_uart_interrupt_service(_handle, 0, (uint32_t *)&_tx_idle);

    // Register class into local variable
    uart_handler = this;

    // Enable UART interrupt
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn));

    is_initialized = true;
}

void Uart::end() {
    if (_handle != nullptr) {
        flush();

        // Power down UART
        am_hal_uart_power_control(_handle, AM_HAL_SYSCTRL_DEEPSLEEP, false);
        am_hal_uart_deinitialize(_handle);

        // Disable UART pins
        am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_HAL_GPIO_DISABLE);
        am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_HAL_GPIO_DISABLE);
    }
}

int Uart::available() {
    int num_bytes;

    NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn));
    num_bytes = _rx_idx;
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn));

    return num_bytes;
}

int Uart::read() {
    int ret_byte;

    NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn));
    ret_byte = _rx_buff[_rx_idx - 1];
    // Shift remaining data to front of buffer
    int remaining{_rx_idx - 1};
    for (int i{}; i < remaining; ++i) {
        _rx_buff[i] = _rx_buff[1 + i];
    }
    _rx_idx = remaining;
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn));

    return ret_byte;
}

int Uart::read(uint8_t *buffer, int len) {
    NVIC_DisableIRQ((IRQn_Type)(UART0_IRQn));
    int num_bytes{(len < _rx_idx ? len : _rx_idx)};
    // Copy into output buffer
    for (int i{}; i < num_bytes; ++i) {
        buffer[i] = _rx_buff[i];
    }
    // Shift remaining data to front of buffer
    int remaining{_rx_idx - num_bytes};
    for (int i{}; i < remaining; ++i) {
        _rx_buff[i] = _rx_buff[num_bytes + i];
    }
    _rx_idx = remaining;
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn));
    return num_bytes;
}

void Uart::flush() { am_hal_uart_tx_flush(_handle); }

uint32_t Uart::write(const uint8_t *buffer, uint32_t size) {
    uint32_t ui32TransferBytesWritten{};
    uint32_t ui32TotalBytesWritten{};
    if (!is_initialized) {
        return 0;
    }
    // Print the string via UART
    am_hal_uart_transfer_t sUartWrite = {
        .ui32Direction = AM_HAL_UART_WRITE,
        .pui8Data = NULL,
        .ui32NumBytes = 0,
        .ui32TimeoutMs = 100,
        .pui32BytesTransferred = &ui32TransferBytesWritten,
    };

    do {
        sUartWrite.pui8Data = (uint8_t *)(buffer + ui32TotalBytesWritten);
        sUartWrite.ui32NumBytes = ((size - ui32TotalBytesWritten) > BUFF_SIZE ? BUFF_SIZE : (size - ui32TotalBytesWritten));
        am_hal_uart_transfer(_handle, &sUartWrite);
        ui32TotalBytesWritten += ui32TransferBytesWritten;
    } while (ui32TotalBytesWritten < size);

    // Wait for all data to be transmitted before returning
    am_hal_uart_tx_flush(_handle);

    return ui32TotalBytesWritten;
}

uint32_t Uart::write(const uint8_t data) {
    if (!is_initialized) {
        return 0;
    }
    return write(&data, 1);
}

uint32_t Uart::printf(const char *pcFmt, ...) {
    uint32_t ui32NumChars{};
    char _print_buff[AM_PRINTF_BUFSIZE]{};

    // Convert to the desired string
    va_list pArgs{};
    va_start(pArgs, pcFmt);
    ui32NumChars = am_util_stdio_vsprintf(_print_buff, pcFmt, pArgs);
    va_end(pArgs);

    if (!is_initialized) {
        return 0;
    }
    write((const uint8_t *)_print_buff, ui32NumChars);

    return ui32NumChars;
}

void Uart::uart_isr() {
    uint32_t ui32Status{};
    uint8_t rx_chunk[128]{};

    // Service FIFOs as necessary, and clear interrupts
    am_hal_uart_interrupt_status_get(_handle, &ui32Status, true);
    am_hal_uart_interrupt_clear(_handle, ui32Status);
    am_hal_uart_interrupt_service(_handle, ui32Status, (uint32_t *)&_tx_idle);

    if (ui32Status & (AM_HAL_UART_INT_RX_TMOUT | AM_HAL_UART_INT_RX)) {
        uint32_t ui32BytesRead{};

        am_hal_uart_transfer_t sRead = {
            .ui32Direction = AM_HAL_UART_READ,
            .pui8Data = rx_chunk,
            .ui32NumBytes = sizeof(rx_chunk),
            .ui32TimeoutMs = 0,
            .pui32BytesTransferred = &ui32BytesRead,
        };
        am_hal_uart_transfer(_handle, &sRead);

        for (uint32_t i{}; i < ui32BytesRead; ++i) {
            _rx_buff[_rx_idx] = rx_chunk[i];
            ++_rx_idx;
        }
    }
}

// ISR
extern "C" void __attribute__((interrupt)) am_uart_isr(void) {
    Serial.uart_isr();
}

// Global UART instance
Uart Serial;