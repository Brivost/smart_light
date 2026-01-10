#ifndef LAYERS_H
#define LAYERS_H

#include "arm_nnfunctions.h"

struct LinearLayer {
    const int32_t act_min{-128};
    const int32_t act_max{127};
    const int32_t mult{};
    const int32_t input_nodes{};
    const int32_t output_nodes{};
    const int8_t *weight;
    const int32_t *bias;
};

struct ConvLayer {
    const int32_t act_min{-128};
    const int32_t act_max{127};
    const int32_t input_channels{};
    const int32_t output_channels{};
    const int32_t input_width{};
    const int32_t input_height{};
    const int32_t output_width{};
    const int32_t output_height{};
    const int32_t kernel_size{};
    const int32_t stride{};
    const int32_t padding{};
    const int32_t dilation{};
    const int32_t *mult{};
    const int8_t *weight;
    const int32_t *bias;
};

void Linear(LinearLayer *layer, int8_t *input, int8_t *output);
void Conv(ConvLayer *layer, int8_t *input, int8_t *output);
#endif