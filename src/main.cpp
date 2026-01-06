//*****************************************************************************
//
//! @file main.cpp
//!
//! @brief Main firmware
//!
//! Purpose: Main firmware
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "uart.h"

void board_init() {
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    am_bsp_low_power_init();
    am_hal_interrupt_master_enable();  // Enable interruts master
    am_hal_gpio_pinconfig(am_bsp_psLEDs[0].ui32GPIONumber, g_AM_HAL_GPIO_OUTPUT);
    am_devices_led_off(am_bsp_psLEDs, 0);
}

void led_on() {
    am_devices_led_on(am_bsp_psLEDs, 0);
}

void led_off() {
    am_devices_led_off(am_bsp_psLEDs, 0);
}

void process_command(const uint8_t data) {
    switch (data) {
        case 'y':
            led_on();
            Serial.printf("Turning LED on\n");
            break;
        case 'n':
            led_off();
            Serial.printf("Turning LED off\n");
            break;
        default:
            Serial.printf("Unrecognized command %d. Enter 'y' or 'n'.\n", data);
            break;
    }
}

int main() {
    board_init();

    Serial.begin(115200);

    Serial.printf("\nEnter 'y' to turn LED on, 'n' to turn LED off.\n");
    while (1) {
        if (Serial.available()) {
            process_command(Serial.read());
        }
    }
}
