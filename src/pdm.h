#ifndef PDM_H
#define PDM_H

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "arm_math.h"

#define PDM_SIZE 4096
#define PDM_BYTES (PDM_SIZE * 2)

#define PCM_SR 16000
#define RECORDING_DURATION_SECONDS 2
// Multiple of PDM_SIZE to ensure whole number of PDM samples are recorded and we do not write past buffer
#define PRINT_BUFF_SIZE ((PCM_SR * RECORDING_DURATION_SECONDS / PDM_SIZE) * PDM_SIZE)

class Pdm {
   public:
    bool is_initialized = false;
    Pdm();
    void begin();
    void deinit();
    void get();
    void reset();
    int getSamplesRecorded();
    int16_t* getPrintBuffer();
    am_hal_pdm_config_t* getConfig();
    bool dataIsReady();
    void setDataReady(bool);
    void pdm_isr();

   private:
    void* _handle;
    bool _PDMDataReady = false;
    uint32_t _PDMDataBuffer[PDM_SIZE];
    int16_t _PCMPrintBuffer[PRINT_BUFF_SIZE];
    int32_t _PCMSamplesRecorded{};
    uint32_t _SampleFreq;
    am_hal_pdm_config_t _config;
};

extern Pdm Mic;

#endif