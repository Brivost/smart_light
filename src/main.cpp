//*****************************************************************************
//
//! @file main.cpp
//!
//! @brief A simple read and write example using UART
//!
//! Purpose: Prints device info over UART. Then accepts 'y' or 'n' commands
//! via UART to turn on or off LED.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "uart.h"

volatile bool g_led_state = false;

void board_init() {
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    am_bsp_low_power_init();
    am_hal_interrupt_master_enable();  // Enable interruts master
}

void led_init() {
    uint32_t pin = am_bsp_psLEDs[0].ui32GPIONumber;
    am_hal_gpio_pinconfig(pin, g_AM_HAL_GPIO_OUTPUT);
    am_devices_led_off(am_bsp_psLEDs, 0);
    g_led_state = false;
}

void led_on() {
    am_devices_led_on(am_bsp_psLEDs, 0);
    g_led_state = true;
}

void led_off() {
    am_devices_led_off(am_bsp_psLEDs, 0);
    g_led_state = false;
}

void process_command(const uint8_t data) {
    switch (data) {
        case 'y':
            led_on();
            uart.printf("Turning LED on\n");
            break;
        case 'n':
            led_off();
            uart.printf("Turning LED off\n");
            break;
        default:
            uart.printf("Unrecognized command. Enter 'y' or 'n'.\n");
            break;
    }
}

void print_device_info() {
    am_util_id_t id;
    am_util_id_device(&id);

    uart.printf("Hello World this is a very super long string that I am trying to print!\n\n");
    uart.printf("Vendor: %s\n", id.pui8VendorName);
    uart.printf("0123456789\n");
    uart.printf("Device: %s\n", id.pui8DeviceName);
    uart.printf("Chip ID: 0x%08X\n", id.sMcuCtrlDevice.ui32ChipID0);
    uart.printf("Revision: Rev%c%c\n", id.ui8ChipRevMaj, id.ui8ChipRevMin);
    uart.printf("\nEnter 'y' to turn LED on, 'n' to turn LED off.\n");
}

int main() {
    board_init();
    led_init();

    uart.begin(115200);

    print_device_info();

    int num_bytes{};
    while (1) {
        num_bytes = uart.available();
        if (num_bytes > 0) {
            process_command(uart.read());
        }
        // uart.printf("Waiting");
        // am_util_delay_ms(1000);
    }
}
