/******************************************************************************
 *
 * Main firmware file for SparkFun Artemis ATP
 *
 * This is a simple blink example that demonstrates basic GPIO control
 * and serves as a starting point for your firmware development.
 *
 ******************************************************************************/

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

// LED pin definition for Artemis ATP
#define LED_PIN 19

//*****************************************************************************
//
// Main function
//
//*****************************************************************************
int main(void) {
    // Set the clock frequency
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation
    am_bsp_low_power_init();

    // Initialize the printf interface for debug output
    am_bsp_uart_printf_enable();

    // Clear the terminal and print the banner
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("SparkFun Artemis ATP - Firmware Example\n");
    am_util_stdio_printf("========================================\n\n");

    // Configure LED GPIO
    am_hal_gpio_pinconfig(LED_PIN, g_AM_HAL_GPIO_OUTPUT_12);
    am_hal_gpio_state_write(LED_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);

    am_util_stdio_printf("LED Blink starting on pin %d\n", LED_PIN);
    am_util_stdio_printf("Press Ctrl+C to stop\n\n");

    // Main loop
    uint32_t count = 0;
    while (1) {
        // Toggle LED
        am_hal_gpio_state_write(LED_PIN, AM_HAL_GPIO_OUTPUT_SET);
        am_util_stdio_printf("LED ON  - Count: %d\n", count);

        // Delay 500ms
        am_util_delay_ms(500);

        // Toggle LED
        am_hal_gpio_state_write(LED_PIN, AM_HAL_GPIO_OUTPUT_CLEAR);
        am_util_stdio_printf("LED OFF - Count: %d\n", count);

        // Delay 500ms
        am_util_delay_ms(500);

        count++;
    }

    return 0;
}
