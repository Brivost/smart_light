#ifndef UART_H
#define UART_H

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"

#define BUFF_SIZE 256

class Uart {
   public:
    bool is_initialized = false;
    Uart();
    void begin(uint32_t baudrate);
    void end();
    int available();
    int read();
    int read(uint8_t *buffer, int len);
    void flush();
    uint32_t write(const uint8_t *buffer, uint32_t size);
    uint32_t write(const uint8_t data);
    uint32_t printf(const char *pcFmt, ...);
    void uart_isr(void);

   private:
    void *_handle;
    am_hal_uart_config_t _config;
    // TODO : Make RingBuffers
    uint8_t _tx_buff[BUFF_SIZE];
    uint8_t _rx_buff[BUFF_SIZE];
    int _rx_idx = 0;
    volatile uint32_t _tx_idle;
};

extern Uart Serial;

#endif  // UART_H