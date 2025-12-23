//*****************************************************************************
//
//! @file main.c
//!
//! @brief A simple read and write example using UART
//!
//! Purpose: Prints device info over UART. Then accepts 'y' or 'n' commands
//  via UART to turn on or off LED.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

// UART handle
void *phUART;

#define CHECK_ERRORS(x)                 \
    if ((x) != AM_HAL_STATUS_SUCCESS) { \
        error_handler(x);               \
    }
#define BLINK_PERIOD 500

volatile uint32_t ui32LastError;
volatile uint32_t g_rxIndex = 0;
volatile bool g_led_state = false;

// Catch HAL errors
void error_handler(uint32_t ui32ErrorStatus) {
    ui32LastError = ui32ErrorStatus;

    while (1);
}

// UART buffers
uint8_t g_pui8TxBuffer[256];
uint8_t g_pui8RxBuffer[1];

// UART configuration
const am_hal_uart_config_t g_sUartConfig =
    {
        // Standard UART settings: 115200-8-N-1
        .ui32BaudRate = 115200,
        .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
        // Set TX and RX FIFOs to interrupt at half-full
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 |
                           AM_HAL_UART_RX_FIFO_1_2),
        // Buffers
        .pui8TxBuffer = g_pui8TxBuffer,
        .ui32TxBufferSize = sizeof(g_pui8TxBuffer),
        .pui8RxBuffer = g_pui8RxBuffer,
        .ui32RxBufferSize = sizeof(g_pui8RxBuffer),
};

void board_init(void) {
    // Set the clock frequency
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation
    am_bsp_low_power_init();
}

void led_init(void) {
    uint32_t ui32GPIONumber = am_bsp_psLEDs[0].ui32GPIONumber;
    am_hal_gpio_pinconfig(ui32GPIONumber, g_AM_HAL_GPIO_OUTPUT);
    am_devices_led_off(am_bsp_psLEDs, 0);
    g_led_state = false;
}

void uart_print(char *pcStr) {
    uint32_t ui32StrLen = 0;
    uint32_t ui32BytesWritten = 0;

    // Measure the length of the string
    while (pcStr[ui32StrLen] != 0) {
        ui32StrLen++;
    }

    // Print the string via the UART
    const am_hal_uart_transfer_t sUartWrite =
        {
            .ui32Direction = AM_HAL_UART_WRITE,
            .pui8Data = (uint8_t *)pcStr,
            .ui32NumBytes = ui32StrLen,
            .ui32TimeoutMs = 0,
            .pui32BytesTransferred = &ui32BytesWritten,
        };

    CHECK_ERRORS(am_hal_uart_transfer(phUART, &sUartWrite));

    if (ui32BytesWritten != ui32StrLen) {
        // Couldn't send the whole string!
        while (1);
    }
}

void uart_init(void *uart_print) {
    // Initialize the printf interface for UART output.
    CHECK_ERRORS(am_hal_uart_initialize(0, &phUART));
    CHECK_ERRORS(am_hal_uart_power_control(phUART, AM_HAL_SYSCTRL_WAKE, false));
    CHECK_ERRORS(am_hal_uart_configure(phUART, &g_sUartConfig));

    // Enable the UART pins
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_TX, g_AM_BSP_GPIO_COM_UART_TX);
    am_hal_gpio_pinconfig(AM_BSP_GPIO_COM_UART_RX, g_AM_BSP_GPIO_COM_UART_RX);

    // Enable interrupts
    NVIC_EnableIRQ((IRQn_Type)(UART0_IRQn + AM_BSP_UART_PRINT_INST));
    am_hal_interrupt_master_enable();

    // Set the main print interface to use the UART print function we defined
    am_util_stdio_printf_init(uart_print);
}

// UART read/write ISR
void am_uart_isr(void) {
    uint32_t ui32Status;
    am_hal_uart_interrupt_status_get(phUART, &ui32Status, true);
    am_hal_uart_interrupt_clear(phUART, ui32Status);
    am_hal_uart_interrupt_service(phUART, ui32Status, NULL);

    // Read available data into your buffer
    if (ui32Status & (AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT)) {
        uint32_t bytesRead;
        am_hal_uart_transfer_t sRead = {
            .ui32Direction = AM_HAL_UART_READ,
            .pui8Data = &g_pui8RxBuffer[g_rxIndex],
            .ui32NumBytes = sizeof(g_pui8RxBuffer) - g_rxIndex,
            .ui32TimeoutMs = 0,
            .pui32BytesTransferred = &bytesRead,
        };
        am_hal_uart_transfer(phUART, &sRead);
        g_rxIndex += bytesRead;
    }
}

void toggle_led(void) {
    g_led_state = !g_led_state;
    (g_led_state) ? am_devices_led_on(am_bsp_psLEDs, 0) : am_devices_led_off(am_bsp_psLEDs, 0);
}

void led_on(void) {
    am_devices_led_on(am_bsp_psLEDs, 0);
}

void led_off(void) {
    am_devices_led_off(am_bsp_psLEDs, 0);
}

// Process UART read buffer
void process_command(uint8_t *pcStr, uint32_t len) {
    // len is number of bytes read
    switch (pcStr[0]) {
        case 'y':
            led_on();
            am_util_stdio_printf("Turning LED on\n");
            break;
        case 'n':
            led_off();
            am_util_stdio_printf("Turning LED off\n");
            break;
        default:
            am_util_stdio_printf("Unrecognized command. Enter 'y' to turn on LED, 'n' to turn off LED.\n");
            break;
    }
}

void print_device_data(void) {
    am_util_id_t sIdDevice;
    uint32_t ui32StrBuf;

    // Print the banner
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Hello World!\n\n");

    // Print the device info
    am_util_id_device(&sIdDevice);
    am_util_stdio_printf("Vendor Name: %s\n", sIdDevice.pui8VendorName);
    am_util_stdio_printf("Device type: %s\n", sIdDevice.pui8DeviceName);

    am_util_stdio_printf("Qualified: %s\n",
                         sIdDevice.sMcuCtrlDevice.ui32Qualified ? "Yes" : "No");

    am_util_stdio_printf(
        "Device Info:\n"
        "\tPart number: 0x%08X\n"
        "\tChip ID0:    0x%08X\n"
        "\tChip ID1:    0x%08X\n"
        "\tRevision:    0x%08X (Rev%c%c)\n",
        sIdDevice.sMcuCtrlDevice.ui32ChipPN,
        sIdDevice.sMcuCtrlDevice.ui32ChipID0,
        sIdDevice.sMcuCtrlDevice.ui32ChipID1,
        sIdDevice.sMcuCtrlDevice.ui32ChipRev,
        sIdDevice.ui8ChipRevMaj, sIdDevice.ui8ChipRevMin);

    // If not a multiple of 1024 bytes, append a plus sign to the KB
    ui32StrBuf = (sIdDevice.sMcuCtrlDevice.ui32FlashSize % 1024) ? '+' : 0;
    am_util_stdio_printf("\tFlash size:  %7d (%d KB%s)\n",
                         sIdDevice.sMcuCtrlDevice.ui32FlashSize,
                         sIdDevice.sMcuCtrlDevice.ui32FlashSize / 1024,
                         &ui32StrBuf);

    ui32StrBuf = (sIdDevice.sMcuCtrlDevice.ui32SRAMSize % 1024) ? '+' : 0;
    am_util_stdio_printf("\tSRAM size:   %7d (%d KB%s)\n\n",
                         sIdDevice.sMcuCtrlDevice.ui32SRAMSize,
                         sIdDevice.sMcuCtrlDevice.ui32SRAMSize / 1024,
                         &ui32StrBuf);

    // Print the compiler version
    am_hal_uart_tx_flush(phUART);
    am_util_stdio_printf("App Compiler:    %s\n", COMPILER_VERSION);
    am_util_stdio_printf("HAL Compiler:    %s\n", g_ui8HALcompiler);
    am_util_stdio_printf("HAL SDK version: %d.%d.%d\n",
                         g_ui32HALversion.s.Major,
                         g_ui32HALversion.s.Minor,
                         g_ui32HALversion.s.Revision);
    am_util_stdio_printf("HAL compiled with %s-style registers\n",
                         g_ui32HALversion.s.bAMREGS ? "AM_REG" : "CMSIS");

    am_hal_security_info_t secInfo;
    char sINFO[32];
    uint32_t ui32Status;
    ui32Status = am_hal_security_get_info(&secInfo);
    if (ui32Status == AM_HAL_STATUS_SUCCESS) {
        if (secInfo.bInfo0Valid) {
            am_util_stdio_sprintf(sINFO, "INFO0 valid, ver 0x%X", secInfo.info0Version);
        } else {
            am_util_stdio_sprintf(sINFO, "INFO0 invalid");
        }

        am_util_stdio_printf("SBL ver: 0x%x - 0x%x, %s\n",
                             secInfo.sblVersion, secInfo.sblVersionAddInfo, sINFO);
    } else {
        am_util_stdio_printf("am_hal_security_get_info failed 0x%X\n", ui32Status);
    }
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void) {
    board_init();
    uart_init(uart_print);
    led_init();

    print_device_data();

    while (1) {
        // Read data from UART
        if (g_rxIndex > 0) {
            // Process received data...
            process_command(g_pui8RxBuffer, g_rxIndex);
            g_rxIndex = 0;
        }
    }
}
