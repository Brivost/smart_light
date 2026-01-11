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

int main() {
    board_init();

    while (1) {
        led_on();
        am_util_delay_ms(500);
        led_off();
        am_util_delay_ms(500);
    }
}
