//*****************************************************************************
//
//! @file main.c
//!
//! @brief A simple LED blinking example.
//!
//
//*****************************************************************************

/*
Copyright (c) 2019 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#define BLINK_PERIOD 500

#define PERIPHERAL_BASE (0x40000000)

// Pad configuration register
#define PADREGB_OFFSET (0x00010004)
#define PADREGB (*(volatile uint32_t *)(PADREGB_OFFSET + PERIPHERAL_BASE))

// AltPad configuration register
#define ALTPADREGB_OFFSET (0x000100E4)
#define ALTPADREGB (*(volatile uint32_t *)(ALTPADREGB_OFFSET + PERIPHERAL_BASE))

// GPIO configuration register
#define CFGA_OFFSET (0x00010040)
#define CFGA (*(volatile uint32_t *)(CFGA_OFFSET + PERIPHERAL_BASE))

// GPIO Output Register
#define WTA_OFFSET (0x00010088)
#define WTA (*(volatile uint32_t *)(WTA_OFFSET + PERIPHERAL_BASE))

// GPIO output set register
#define WTSA_OFFSET (0x00010090)
#define WTSA (*(volatile uint32_t *)(WTSA_OFFSET + PERIPHERAL_BASE))

// GPIO output clear register
#define WTCA_OFFSET (0x0001009C)
#define WTCA (*(volatile uint32_t *)(WTCA_OFFSET + PERIPHERAL_BASE))

// GPIO output enable register
#define ENA_OFFSET (0x000100A0)
#define ENA (*(volatile uint32_t *)(ENA_OFFSET + PERIPHERAL_BASE))

// GPIO output enable set register
#define ENSA_OFFSET (0x000100A8)
#define ENSA (*(volatile uint32_t *)(ENSA_OFFSET + PERIPHERAL_BASE))

// GPIO output enable clear register
#define ENCA_OFFSET (0x000100B4)
#define ENCA (*(volatile uint32_t *)(ENCA_OFFSET + PERIPHERAL_BASE))

// Pad key register
#define PADKEY_OFFSET (0x00010060)
#define PADKEY (*(volatile uint32_t *)(PERIPHERAL_BASE + PADKEY_OFFSET))

/*
 * Steps for using GPIO
 * 1. Set PADKEY to 0x73 in order to write the PADREGB register
 * 2. Configure pad as GPIO by setting bits 13:11 (the PADnFNCSEL field) of PADREGB to 0x3 (011)
 * 4. Configure GPIO as output by setting bits 22:21 of CFGA as 0x3 for push-pull mode
 * 3. Set GPIO drive strength to 2mA by setting bit 8 of ALTPADREGB to 0 and bit 10 of PADREGB to 0
 * 4. Clear PADKEY by setting it to 0x00
 * 5. Enable GPIO by setting bit 5 ENA to 1
 * 6. To turn LED off, set bit 5 of WTCA to 1
 * 7. To turn LED on, set bit 5 of WTSA to 1
 */

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void) {
    // Set the clock frequency.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.
    am_bsp_low_power_init();

    // Set PADKEY
    PADKEY = 0x73;

    // Clear bits 13:11 of PADREGB
    PADREGB &= ~(0x7u << 11);
    // Set bits 13:11 to 0x3
    PADREGB |= (0x3u << 11);

    // Clear bits 22:21 of CFGA
    CFGA &= ~(0x3u << 21);
    // Set bits 22:21 to 0x3
    CFGA |= (0x3u << 21);

    // Set bit 10 to 0 of PADREGB
    PADREGB &= ~(0x1u << 10);

    // Set bit 8 to 0 of ALTPADREGB
    ALTPADREGB &= ~(0x1u << 8);

    // Clear PADKEY
    PADKEY = 0;

    // Set bit 5 of ENA to 1
    ENA |= (0x1u << 5);

    // Set bit 5 of WTA to 0
    WTA &= ~(0x1u << 5);

    // Main loop - toggle LED forever
    while (1) {
        // Toggle bit 5 of WTA
        WTA ^= (0x1u << 5);
        am_util_delay_ms(BLINK_PERIOD);
    }
}
