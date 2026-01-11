#include "layers.h"

void Linear(const LinearLayer *layer, int8_t *input, int8_t *output) {
    cmsis_nn_context ctx{.buf = nullptr, .size = 0};
    cmsis_nn_activation activation{.min = layer->act_min, .max = layer->act_max};
    cmsis_nn_fc_params fc_params{.activation = activation};
    cmsis_nn_per_tensor_quant_params quant_params{.multiplier = layer->mult, .shift = 0};
    cmsis_nn_dims input_dims{.n = 1};
    cmsis_nn_dims filter_dims{.n = layer->input_nodes, .c = layer->output_nodes};
    cmsis_nn_dims bias_dims{.c = layer->output_nodes};
    cmsis_nn_dims output_dims{.c = layer->output_nodes};

    arm_fully_connected_s8(&ctx, &fc_params, &quant_params, &input_dims, input, &filter_dims, layer->weight, &bias_dims, layer->bias, &output_dims, output);
}

// TODO: Declare more of these upfront instead of repeatedly
void Conv(const ConvLayer *layer, int8_t *input, int8_t *output) {
    cmsis_nn_dims input_dims{.n = 1, .h = layer->input_height, .w = layer->input_width, .c = layer->input_channels};
    cmsis_nn_dims filter_dims{.n = layer->output_channels, .h = layer->kernel_size, .w = layer->kernel_size, .c = layer->input_channels};
    cmsis_nn_tile stride{.w = layer->stride, .h = layer->stride};
    cmsis_nn_tile padding{.w = layer->padding, .h = layer->padding};
    cmsis_nn_tile dilation{.w = layer->dilation, .h = layer->dilation};
    cmsis_nn_activation activation{.min = layer->act_min, .max = layer->act_max};
    int32_t size = arm_convolve_s8_get_buffer_size(&input_dims, &filter_dims);
    int16_t buf[size];
    cmsis_nn_context ctx{.buf = buf, .size = size};
    cmsis_nn_conv_params conv_params{.stride = stride, .padding = padding, .dilation = dilation, .activation = activation};
    int32_t shift[layer->output_channels];
    int32_t mult[layer->output_channels];
    for (int i{}; i < layer->output_channels; ++i) {
        mult[i] = layer->mult;
        shift[i] = 0;
    }
    cmsis_nn_per_channel_quant_params quant_params{.multiplier = mult, .shift = shift};
    cmsis_nn_dims bias_dims{};
    cmsis_nn_dims upscale_dims{};
    cmsis_nn_dims output_dims{.h = layer->output_height, .w = layer->output_width, .c = layer->output_channels};

    arm_convolve_s8(&ctx, &conv_params, &quant_params, &input_dims, input, &filter_dims, layer->weight, &bias_dims, layer->bias, &upscale_dims, &output_dims, output);
}
