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
#include "arm_nnfunctions.h"
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

int main() {
    board_init();
    Serial.begin(115200);

    constexpr int32_t input_nodes = 10;
    constexpr int32_t output_nodes = 15;

    const int8_t input[] = {104, 80, 84, 70, 88, 48, 38, 4, 16, 91};
    const int8_t kernel[] = {-1, 69, -105, -94, -49, 34, -3, 101, -11, 34, -39, -25,
                             -122, -85, -53, 5, 51, 77, -87, -56, 46, 106, -26, 96,
                             -21, 14, 116, -119, -80, -32, -50, 110, -83, -59, -89, -120,
                             -75, 110, 57, 62, 7, -66, 22, -119, -92, -66, 81, 75,
                             -57, -5, 82, 127, 51, 17, 86, -75, 24, -99, -89, -66,
                             58, 51, -76, 39, 70, -16, 5, 30, 79, 123, -98, -47,
                             50, 106, 111, 113, 25, -111, 12, -80, -119, 114, 97, -128,
                             24, -22, -21, -59, 49, -76, 47, 65, 91, 48, -127, -83,
                             64, 27, -100, -74, 120, 86, -56, -32, -122, -2, -96, -99,
                             -7, 19, -52, 76, -78, 116, 88, -108, -32, 6, 19, 30,
                             50, 8, -62, 60, -123, -76, -32, -62, -45, -105, -27, 27,
                             -83, -7, 91, -13, 4, -11, 26, 81, 121, 81, 121, -9,
                             -115, -61, 87, -1, -64, -98};
    const int32_t bias[output_nodes] = {};
    int8_t output[output_nodes] = {};

    void *ctx_buff;
    const cmsis_nn_context ctx{.buf = ctx_buff, .size = 0};
    cmsis_nn_activation activation{.min = -128, .max = 127};
    const cmsis_nn_fc_params fc_params{.input_offset = 0, .filter_offset = 0, .output_offset = 0, activation = activation};
    const cmsis_nn_per_tensor_quant_params quant_params{.multiplier = 6485986, .shift = 0};
    const cmsis_nn_dims input_dims{.n = 1};
    const cmsis_nn_dims filter_dims{.n = input_nodes, .c = output_nodes};
    const cmsis_nn_dims bias_dims{.c = output_nodes};
    const cmsis_nn_dims output_dims{.c = output_nodes};

    arm_fully_connected_s8(&ctx, &fc_params, &quant_params, &input_dims, input, &filter_dims, kernel, &bias_dims, bias, &output_dims, output);

    Serial.printf("\n");
    for (int i = 0; i < output_nodes; ++i) {
        Serial.printf("%d, ", output[i]);
    }
    Serial.printf("\n");

    while (1) {
    }
}
