

#ifndef KLINE_H
#define KLINE_H

#include "error_codes.h"

void kline_uart_init(uint16_t baudrate);
void kline_io_init(void);
void kline_task(void);
void kline_display_task(void);


#endif
