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
#include "pdm.h"
#include "uart.h"

//*****************************************************************************
//
// Print PCM from recorded PCM buffer
//
//*****************************************************************************
void pcm_print_all(Pdm &Mic) {
    //
    // Convert the PDM samples to floats
    //
    for (int i = 0; i < Mic.getSamplesRecorded(); i++) {
        Serial.printf("%d\n", Mic.getPrintBuffer()[i]);
    }
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int main(void) {
    // Perform the standard initialzation for clocks, cache settings, and
    // board-level low-power operation.
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();
    am_bsp_low_power_init();
    am_hal_interrupt_master_enable();

    Serial.begin(115200);

    // Turn on the PDM, set it up for our chosen recording settings, and start
    // the first DMA transaction.
    Mic.begin();
    Mic.get();

    // Loop forever, recording and printing bursts of audio
    while (1) {
        if ((Serial.available()) && (Serial.read() == 'r')) {
            Mic.reset();
            am_devices_led_on(am_bsp_psLEDs, 0);
            while (Mic.getSamplesRecorded() + PDM_SIZE <= PRINT_BUFF_SIZE) {
                if (Mic.dataIsReady()) {
                    Mic.setDataReady(false);

                    // Start converting the next set of PCM samples.
                    Mic.get();
                }
            }
            am_devices_led_off(am_bsp_psLEDs, 0);

            // Print PCM data
            pcm_print_all(Mic);
        }
    }
}
