//*****************************************************************************
//
//! @file pdm_fft.c
//!
//! @brief An example to show basic PDM operation.
//!
//! Purpose: This example enables the PDM interface to record audio signals from an
//! external microphone. The required pin connections are:
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! GPIO 10 - PDM DATA
//! GPIO 11 - PDM CLK
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Ambiq Micro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision v2.2.0-7-g63f7c2ba1 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#define ARM_MATH_CM4
#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "arm_math.h"
#include "uart.h"

//*****************************************************************************
//
// Example parameters.
//
//*****************************************************************************
#define PDM_SIZE 4096
#define PDM_BYTES (PDM_SIZE * 2)
#define PRINT_PDM_DATA 1

#define PCM_SR 16000
#define RECORDING_DURATION_SECONDS 5
// Multiple of PDM_SIZE to ensure whole number of PDM samples are recorded and we do not write past buffer
#define PRINT_BUFF_SIZE ((PCM_SR * RECORDING_DURATION_SECONDS / PDM_SIZE) * PDM_SIZE)

//*****************************************************************************
//
// Global variables.
//
//*****************************************************************************
volatile bool g_bPDMDataReady = false;
uint32_t g_ui32PDMDataBuffer[PDM_SIZE];
int16_t g_PCMPrintBuffer[PRINT_BUFF_SIZE];
int32_t g_PCMSamplesRecorded{};
uint32_t g_ui32SampleFreq;

//*****************************************************************************
//
// PDM configuration information.
//
//*****************************************************************************
void *PDMHandle;

am_hal_pdm_config_t g_sPdmConfig =
    {
        .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
        .eLeftGain = AM_HAL_PDM_GAIN_P405DB,
        .eRightGain = AM_HAL_PDM_GAIN_P405DB,
        .ui32DecimationRate = 48,
        .bHighPassEnable = 0,
        .ui32HighPassCutoff = 0xB,
        .ePDMClkSpeed = AM_HAL_PDM_CLK_1_5MHZ,
        .bInvertI2SBCLK = 0,
        .ePDMClkSource = AM_HAL_PDM_INTERNAL_CLK,
        .bPDMSampleDelay = 0,
        .bDataPacking = 1,
        .ePCMChannels = AM_BSP_PDM_CHANNEL,
        .ui32GainChangeDelay = 1,
        .bI2SEnable = 0,
        .bSoftMute = 0,
        .bLRSwap = 0,
};

//*****************************************************************************
//
// PDM initialization.
//
//*****************************************************************************
void pdm_init(void) {
    //
    // Initialize, power-up, and configure the PDM.
    //
    am_hal_pdm_initialize(0, &PDMHandle);
    am_hal_pdm_power_control(PDMHandle, AM_HAL_PDM_POWER_ON, false);
    am_hal_pdm_configure(PDMHandle, &g_sPdmConfig);
    am_hal_pdm_fifo_flush(PDMHandle);

    //
    // Configure the necessary pins.
    //
    am_hal_gpio_pinconfig(AM_BSP_PDM_DATA_PIN, g_AM_BSP_PDM_DATA);
    am_hal_gpio_pinconfig(AM_BSP_PDM_CLOCK_PIN, g_AM_BSP_PDM_CLOCK);

    //
    // Configure and enable PDM interrupts (set up to trigger on DMA
    // completion).
    //
    am_hal_pdm_interrupt_enable(PDMHandle, (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    NVIC_EnableIRQ(PDM_IRQn);
    am_hal_pdm_enable(PDMHandle);
}

//*****************************************************************************
//
// Print PDM configuration data.
//
//*****************************************************************************
void pdm_config_print(void) {
    uint32_t ui32PDMClk;
    uint32_t ui32MClkDiv;

    //
    // Read the config structure to figure out what our internal clock is set
    // to.
    //
    switch (g_sPdmConfig.eClkDivider) {
        case AM_HAL_PDM_MCLKDIV_4:
            ui32MClkDiv = 4;
            break;
        case AM_HAL_PDM_MCLKDIV_3:
            ui32MClkDiv = 3;
            break;
        case AM_HAL_PDM_MCLKDIV_2:
            ui32MClkDiv = 2;
            break;
        case AM_HAL_PDM_MCLKDIV_1:
            ui32MClkDiv = 1;
            break;

        default:
            ui32MClkDiv = 0;
    }

    switch (g_sPdmConfig.ePDMClkSpeed) {
        case AM_HAL_PDM_CLK_12MHZ:
            ui32PDMClk = 12000000;
            break;
        case AM_HAL_PDM_CLK_6MHZ:
            ui32PDMClk = 6000000;
            break;
        case AM_HAL_PDM_CLK_3MHZ:
            ui32PDMClk = 3000000;
            break;
        case AM_HAL_PDM_CLK_1_5MHZ:
            ui32PDMClk = 1500000;
            break;
        case AM_HAL_PDM_CLK_750KHZ:
            ui32PDMClk = 750000;
            break;
        case AM_HAL_PDM_CLK_375KHZ:
            ui32PDMClk = 375000;
            break;
        case AM_HAL_PDM_CLK_187KHZ:
            ui32PDMClk = 187000;
            break;

        default:
            ui32PDMClk = 0;
    }

    //
    // Record the effective sample frequency. We'll need it later to print the
    // loudest frequency from the sample.
    //
    g_ui32SampleFreq = (ui32PDMClk /
                        (ui32MClkDiv * 2 * g_sPdmConfig.ui32DecimationRate));

    Serial.printf("Settings:\n");
    Serial.printf("PDM Clock (Hz):         %12d\n", ui32PDMClk);
    Serial.printf("Decimation Rate:        %12d\n", g_sPdmConfig.ui32DecimationRate);
    Serial.printf("Effective Sample Freq.: %12d\n", g_ui32SampleFreq);
}

//*****************************************************************************
//
// Start a transaction to get some number of bytes from the PDM interface.
//
//*****************************************************************************
void pdm_data_get(void) {
    //
    // Configure DMA and target address.
    //
    am_hal_pdm_transfer_t sTransfer;
    sTransfer.ui32TargetAddr = (uint32_t)g_ui32PDMDataBuffer;
    sTransfer.ui32TotalCount = PDM_BYTES;

    //
    // Start the data transfer.
    //
    am_hal_pdm_dma_start(PDMHandle, &sTransfer);
}

//*****************************************************************************
//
// PDM interrupt handler.
//
//*****************************************************************************
extern "C" void am_pdm0_isr(void) {
    uint32_t ui32Status;

    //
    // Read the interrupt status.
    //
    am_hal_pdm_interrupt_status_get(PDMHandle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(PDMHandle, ui32Status);

    //
    // Copy PDM data to print buffer
    //
    if (ui32Status & AM_HAL_PDM_INT_DCMP) {
        g_bPDMDataReady = true;
        memcpy(g_PCMPrintBuffer + g_PCMSamplesRecorded, (int16_t *)g_ui32PDMDataBuffer, PDM_BYTES);
        g_PCMSamplesRecorded += PDM_SIZE;
    }
}

//*****************************************************************************
//
// Print PCM from single PDM buffer length
//
//*****************************************************************************
void pcm_print(void) {
    int16_t *pi16PDMData = (int16_t *)g_ui32PDMDataBuffer;

    //
    // Convert the PDM samples to floats
    //
    for (uint32_t i = 0; i < PDM_SIZE; i++) {
        Serial.printf("%d\n", pi16PDMData[i]);
    }
}

//*****************************************************************************
//
// Print PCM from recorded PCM buffer
//
//*****************************************************************************
void pcm_print_all(void) {
    //
    // Convert the PDM samples to floats
    //
    for (int i = 0; i < g_PCMSamplesRecorded; i++) {
        Serial.printf("%d\n", g_PCMPrintBuffer[i]);
    }
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void) {
    //
    // Perform the standard initialzation for clocks, cache settings, and
    // board-level low-power operation.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    am_bsp_low_power_init();
    am_hal_interrupt_master_enable();

    Serial.begin(115200);

    //
    // Turn on the PDM, set it up for our chosen recording settings, and start
    // the first DMA transaction.
    //
    pdm_init();
    pdm_data_get();

    //
    // Loop forever
    //
    while (g_PCMSamplesRecorded + PDM_SIZE <= PRINT_BUFF_SIZE) {
        if (g_bPDMDataReady) {
            g_bPDMDataReady = false;

            // Start converting the next set of PCM samples.
            pdm_data_get();
        }
    }
    pcm_print_all();
}
