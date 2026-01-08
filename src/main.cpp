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
#include "layers.h"
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

void print_linear_output(int8_t *output, int32_t output_nodes) {
    for (int i = 0; i < output_nodes; ++i) {
        Serial.printf("%d, ", output[i]);
    }
}

int main() {
    board_init();
    Serial.begin(115200);

    constexpr int32_t input_channels{1};
    constexpr int32_t output_channels{1};
    constexpr int32_t input_width{3};
    constexpr int32_t input_height{3};
    constexpr int32_t output_width{input_width};
    constexpr int32_t output_height{input_height};
    constexpr int32_t kernel_width{3};
    constexpr int32_t kernel_height{3};
    cmsis_nn_tile stride{.w = 1, .h = 1};
    cmsis_nn_tile padding{.w = 1, .h = 1};
    cmsis_nn_tile dilation{.w = 1, .h = 1};
    cmsis_nn_activation activation{.min = -128, .max = 127};
    int32_t mults[output_channels];
    for (int i{}; i < output_channels; ++i) {
        mults[i] = 10893229;
    }
    int32_t shift[output_channels];
    int8_t input_data[] = {75, 91, 89, 56, 12, 54, 86, 41, 88};
    const int8_t filter_data[] = {-1, 83, -128, -114, -60, 42, -3, 123, -14};
    const int32_t bias_data[output_channels]{};
    int8_t output_data[output_channels * output_width * output_height];

    const cmsis_nn_dims input_dims{.n = 1, .h = input_height, .w = input_width, .c = input_channels};
    const cmsis_nn_dims filter_dims{.n = output_channels, .h = kernel_height, .w = kernel_width, .c = input_channels};
    int32_t size = arm_convolve_s8_get_buffer_size(&input_dims, &filter_dims);
    int16_t buf[size];
    const cmsis_nn_context ctx{.buf = buf, .size = size};
    const cmsis_nn_conv_params conv_params{.input_offset = 0, .output_offset = 0, .stride = stride, .padding = padding, .dilation = dilation, .activation = activation};
    const cmsis_nn_per_channel_quant_params quant_params{.multiplier = mults, .shift = shift};
    const cmsis_nn_dims bias_dims{};
    const cmsis_nn_dims upscale_dims{};
    const cmsis_nn_dims output_dims{.h = output_height, .w = output_width, .c = output_channels};

    arm_convolve_s8(&ctx, &conv_params, &quant_params, &input_dims, input_data, &filter_dims, filter_data, &bias_dims, bias_data, &upscale_dims, &output_dims, output_data);

    print_linear_output(output_data, output_channels * output_width * output_height);

    while (1) {
    }
}
