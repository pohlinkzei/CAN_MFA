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

#define INT6_RISING 0x30
#define INT6_FALLING 0x20



volatile uint16_t k58b_timer;
volatile uint8_t k58b_scale;
uint16_t volatile k58b_pw;
void initk58_pwm(void);


#endif