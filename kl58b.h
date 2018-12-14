/*
 * kl58b.c
 *
 * Created: 15.10.2014 10:13:55
 *  Author: Hubert
 # Frequenz K58B: 72,36Hz
 # Tastverhältnis: 20-100%
 */ 
#ifndef KL58B_H
#define KL58B_H
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>


extern volatile uint16_t k58b_timer;
extern volatile uint8_t k58b_pw; // prozentwert fuer pwm
extern volatile uint8_t debug_ocr1b;

void initk58_pwm(void);
void set_backlight(uint8_t pc);

#endif