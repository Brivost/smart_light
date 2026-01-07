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

    constexpr int32_t input_nodes = 3;
    constexpr int32_t output_nodes = 2;

    const int8_t input[] = {60, 74, 38};
    const int8_t kernel[] = {-1, 83, -127, -114, -60, 42};
    const int32_t bias[] = {0, 0};
    int8_t output[output_nodes] = {};

    void *ctx_buff;
    const cmsis_nn_context ctx{.buf = ctx_buff, .size = 0};
    cmsis_nn_activation activation{.min = -128, .max = 127};
    const cmsis_nn_fc_params fc_params{.input_offset = 0, .filter_offset = 0, .output_offset = 0, activation = activation};
    const cmsis_nn_per_tensor_quant_params quant_params{.multiplier = 13372818, .shift = 0};
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
