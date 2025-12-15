/******************************************************************************
*
* Board Support Package implementation for SparkFun Artemis ATP
* 
******************************************************************************/

#include "am_bsp.h"
#include "am_util.h"

// UART handle for printf
static void *g_pvUART;

//*****************************************************************************
//
// Initialize hardware for low power operation
//
//*****************************************************************************
void am_bsp_low_power_init(void)
{
    // Enable internal buck converters for optimal power efficiency
    am_hal_pwrctrl_low_power_init();

    // Disable unused peripherals
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_IOS);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_ADC);
    am_hal_pwrctrl_periph_disable(AM_HAL_PWRCTRL_PERIPH_PDM);
}

//*****************************************************************************
//
// Enable UART printf functionality
//
//*****************************************************************************
void am_bsp_uart_printf_enable(void)
{
    am_hal_uart_config_t sUartConfig =
    {
        .ui32BaudRate = 115200,
        .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
        .ui32Parity = AM_HAL_UART_PARITY_NONE,
        .ui32StopBits = AM_HAL_UART_ONE_STOP_BIT,
        .ui32FlowControl = AM_HAL_UART_FLOW_CTRL_NONE,
        .ui32FifoLevels = (AM_HAL_UART_TX_FIFO_1_2 | AM_HAL_UART_RX_FIFO_1_2),
    };

    // Initialize the UART module
    am_hal_uart_initialize(AM_BSP_UART_PRINT_INST, &g_pvUART);
    am_hal_uart_power_control(g_pvUART, AM_HAL_SYSCTRL_WAKE, false);
    am_hal_uart_configure(g_pvUART, &sUartConfig);

    // Configure GPIO pins for UART
    am_hal_gpio_pinconfig(AM_BSP_UART_TX_PIN, g_AM_HAL_GPIO_COM_UART0_TX);
    am_hal_gpio_pinconfig(AM_BSP_UART_RX_PIN, g_AM_HAL_GPIO_COM_UART0_RX);

    // Register the UART with the stdio system
    am_util_stdio_printf_init(am_hal_uart_transfer_complete);
}

//*****************************************************************************
//
// Disable UART printf functionality
//
//*****************************************************************************
void am_bsp_uart_printf_disable(void)
{
    // Power down the UART
    am_hal_uart_power_control(g_pvUART, AM_HAL_SYSCTRL_DEEPSLEEP, false);
    am_hal_uart_deinitialize(g_pvUART);

    // Disable the GPIO pins
    am_hal_gpio_pinconfig(AM_BSP_UART_TX_PIN, g_AM_HAL_GPIO_DISABLE);
    am_hal_gpio_pinconfig(AM_BSP_UART_RX_PIN, g_AM_HAL_GPIO_DISABLE);
}

//*****************************************************************************
//
// Enable debug printf (ITM)
//
//*****************************************************************************
void am_bsp_debug_printf_enable(void)
{
    am_bsp_itm_printf_enable();
}

//*****************************************************************************
//
// Disable debug printf (ITM)
//
//*****************************************************************************
void am_bsp_debug_printf_disable(void)
{
    am_bsp_itm_printf_disable();
}

