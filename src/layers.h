#ifndef LAYERS_H
#define LAYERS_H

#include "arm_nnfunctions.h"

struct LinearLayer {
    int32_t act_min{-128};
    int32_t act_max{127};
    int32_t mult{};
    int32_t input_nodes{};
    int32_t output_nodes{};
    const int8_t *weight;
    const int32_t *bias;
};

struct ConvLayer {
    int32_t act_min{-128};
    int32_t act_max{127};
    int32_t input_channels{};
    int32_t output_channels{};
    int32_t input_width{};
    int32_t input_height{};
    int32_t output_width{};
    int32_t output_height{};
    int32_t kernel_size{};
    int32_t stride{};
    int32_t padding{};
    int32_t dilation{};
    int32_t mult{};
    const int8_t *weight;
    const int32_t *bias;
};

void Linear(const LinearLayer *layer, int8_t *input, int8_t *output);
void Conv(const ConvLayer *layer, int8_t *input, int8_t *output);
#endif