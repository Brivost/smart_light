#include "pdm.h"

Pdm *pdm_handler = 0;

Pdm::Pdm() {
    _handle = nullptr;
}

void Pdm::begin() {
    _config =
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

    // Initialize, power-up, and configure the PDM
    if (_handle == nullptr) {
        am_hal_pdm_initialize(0, &_handle);
    }
    am_hal_pdm_power_control(_handle, AM_HAL_PDM_POWER_ON, false);
    am_hal_pdm_configure(_handle, &_config);
    am_hal_pdm_fifo_flush(_handle);

    // Configure the necessary pins
    am_hal_gpio_pinconfig(AM_BSP_PDM_DATA_PIN, g_AM_BSP_PDM_DATA);
    am_hal_gpio_pinconfig(AM_BSP_PDM_CLOCK_PIN, g_AM_BSP_PDM_CLOCK);

    // Configure and enable PDM interrupts (set up to trigger on DMA completion)
    am_hal_pdm_interrupt_enable(_handle, (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    pdm_handler = this;

    NVIC_EnableIRQ(PDM_IRQn);
    am_hal_pdm_enable(_handle);

    is_initialized = true;
}

void Pdm::deinit() {
    if (_handle == NULL)
        return;

    am_hal_pdm_interrupt_clear(_handle, (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    am_hal_pdm_interrupt_disable(_handle, (AM_HAL_PDM_INT_DERR | AM_HAL_PDM_INT_DCMP | AM_HAL_PDM_INT_UNDFL | AM_HAL_PDM_INT_OVF));

    NVIC_DisableIRQ(PDM_IRQn);

    am_hal_gpio_pinconfig(AM_BSP_PDM_DATA_PIN, g_AM_HAL_GPIO_DISABLE);
    am_hal_gpio_pinconfig(AM_BSP_PDM_CLOCK_PIN, g_AM_HAL_GPIO_DISABLE);

    // This was a workaround code to measure power consumption. Need to review!!
    am_app_KWD_pdm_dma_disable(_handle);

    am_hal_pdm_disable(_handle);
    am_hal_pdm_power_control(_handle, AM_HAL_PDM_POWER_OFF, false);

    am_hal_pdm_deinitialize(_handle);
    _handle = nullptr;
}

// Start a transaction to get some number of bytes from the PDM interface.
void Pdm::get() {
    // Configure DMA and target address.
    am_hal_pdm_transfer_t sTransfer;
    sTransfer.ui32TargetAddr = (uint32_t)_PDMDataBuffer;
    sTransfer.ui32TotalCount = PDM_BYTES;

    // Start the data transfer.
    am_hal_pdm_dma_start(_handle, &sTransfer);
}

void Pdm::reset() {
    NVIC_DisableIRQ(PDM_IRQn);
    memset(_PCMPrintBuffer, 0, sizeof(_PCMPrintBuffer));
    memset(_PDMDataBuffer, 0, sizeof(_PDMDataBuffer));
    _PCMSamplesRecorded = 0;
    NVIC_EnableIRQ(PDM_IRQn);
}

int Pdm::getSamplesRecorded() {
    return _PCMSamplesRecorded;
}

int16_t *Pdm::getPrintBuffer() {
    return _PCMPrintBuffer;
}

void Pdm::pdm_isr() {
    uint32_t ui32Status;

    //
    // Read the interrupt status.
    //
    am_hal_pdm_interrupt_status_get(_handle, &ui32Status, true);
    am_hal_pdm_interrupt_clear(_handle, ui32Status);

    //
    // Copy PDM data to print buffer
    //
    if (ui32Status & AM_HAL_PDM_INT_DCMP) {
        _PDMDataReady = true;
        memcpy(_PCMPrintBuffer + _PCMSamplesRecorded, (int16_t *)_PDMDataBuffer, PDM_BYTES);
        _PCMSamplesRecorded += PDM_SIZE;
    }
}

extern "C" void __attribute__((interrupt)) am_pdm0_isr(void) {
    Pdm.pdm_isr();
}

Pdm Mic;