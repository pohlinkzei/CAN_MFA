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

#define INT4_RISING 0x03
#define INT4_FALLING 0x02


#define K58B_POLL 0

extern uint8_t volatile k58b_status; // fuer alten wert (Flankenerkennung)
extern uint16_t volatile k58b_high; // zaehler für 0.1ms high samples
extern uint16_t volatile k58b_low; // zaehler für 0.1ms low samples
extern volatile uint16_t k58b_timer;
extern volatile uint8_t k58b_scale;
extern volatile uint16_t k58b_pw; // prozentwert fuer pwm
void initk58_pwm(void);
void set_backlight(uint8_t pc);

#endif