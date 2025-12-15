/******************************************************************************
 *
 * Board Support Package for SparkFun Artemis ATP
 *
 ******************************************************************************/

#ifndef AM_BSP_H
#define AM_BSP_H

#include <stdbool.h>
#include <stdint.h>

#include "am_devices.h"
#include "am_mcu_apollo.h"

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// Board-specific pin definitions
//
//*****************************************************************************

// LED definitions
#define AM_BSP_GPIO_LED0 19
#define AM_BSP_GPIO_LED1 18
#define AM_BSP_GPIO_LED2 17
#define AM_BSP_GPIO_LED3 16
#define AM_BSP_GPIO_LED4 15

// Button definitions
#define AM_BSP_GPIO_BUTTON0 16
#define AM_BSP_GPIO_BUTTON1 18
#define AM_BSP_GPIO_BUTTON2 19

// UART definitions (for debug)
#define AM_BSP_UART_PRINT_INST 0
#define AM_BSP_UART_TX_PIN 22
#define AM_BSP_UART_RX_PIN 23

// I2C definitions
#define AM_BSP_I2C_IOM_MODULE 4
#define AM_BSP_GPIO_IOM4_SCL 39
#define AM_BSP_GPIO_IOM4_SDA 40

// SPI definitions
#define AM_BSP_SPI_IOM_MODULE 0
#define AM_BSP_GPIO_IOM0_SCK 5
#define AM_BSP_GPIO_IOM0_MISO 6
#define AM_BSP_GPIO_IOM0_MOSI 7
#define AM_BSP_GPIO_IOM0_CS 11

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************

extern void am_bsp_low_power_init(void);
extern void am_bsp_uart_printf_enable(void);
extern void am_bsp_uart_printf_disable(void);
extern void am_bsp_debug_printf_enable(void);
extern void am_bsp_debug_printf_disable(void);

#ifdef __cplusplus
}
#endif

#endif  // AM_BSP_H
