#include "layers.h"

void Linear(LinearLayer *layer, int8_t *input, int8_t *output) {
    void *ctx_buff;
    const cmsis_nn_context ctx{.buf = ctx_buff, .size = 0};
    cmsis_nn_activation activation{.min = layer->act_min, .max = layer->act_max};
    const cmsis_nn_fc_params fc_params{.input_offset = 0, .filter_offset = 0, .output_offset = 0, activation = activation};
    const cmsis_nn_per_tensor_quant_params quant_params{.multiplier = layer->mult, .shift = 0};
    const cmsis_nn_dims input_dims{.n = 1};
    const cmsis_nn_dims filter_dims{.n = layer->input_nodes, .c = layer->output_nodes};
    const cmsis_nn_dims bias_dims{.c = layer->output_nodes};
    const cmsis_nn_dims output_dims{.c = layer->output_nodes};

    arm_fully_connected_s8(&ctx, &fc_params, &quant_params, &input_dims, input, &filter_dims, layer->weight, &bias_dims, layer->bias, &output_dims, output);
}