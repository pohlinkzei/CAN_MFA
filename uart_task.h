/*
 * uart_task.h
 *
 * Created: 11.01.2017 22:07:12
 *  Author: Hubert
 */ 


#ifndef UART_TASK_H_
#define UART_TASK_H_

#define UART_BAUD_RATE	9600
void uart_bootloader_init(uint32_t baudrate);
void uart_bootloader_task(void);

extern void disable_interrupts(void);

#endif /* UART_TASK_H_ */