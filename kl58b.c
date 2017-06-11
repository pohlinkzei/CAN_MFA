/*
 * kl58b.c
 *
 * Created: 15.10.2014 10:13:55
 *  Author: Hubert
 # Frequenz K58B: 72,36Hz
 # Tastverhältnis: 20-100%
 */
#include <avr/eeprom.h>
#include "kl58b.h"
#include "CAN-MFA.h"

uint8_t volatile k58b_status; // fuer alten wert (Flankenerkennung)
uint16_t volatile k58b_high;
uint16_t volatile k58b_low;
volatile uint16_t k58b_timer;
volatile uint8_t k58b_scale;
uint16_t volatile k58b_pw;
uint8_t volatile bel_pwm = 255;
uint8_t volatile bel_tim = 0;

void initk58_pwm(void){
	k58b_high = 0;
	k58b_low = 0;
	k58b_pw = 100;
	k58b_timer = 0; 
	#if K58B_POLL
	#warning "POLL k58b signal"
	#else
	EICRB |= INT6_RISING;  
	EIMSK |= (1<<INT6);
	#endif
	
	TCCR3A = 0x00;
	TCCR3A |= (1<<COM3A1);	// Clear on match (nicht-invertierte PWM)
	TCCR3A |= (1<<WGM30);	// für WGM 1
		
	OCR3A = 0x00;
	TCNT3 = 0x00;
	TCCR3B = 0x00;
	//TCCR1B |= (1<<CS11);	// Teiler 8
	TCCR3B |= (1<<CS30) | (1<<CS31);	// Teiler 64 für ca. 60 Hz bei 2 Mhz
	//TCCR3B |= (1<<CS31);	// Teiler 8 für ca. 245 Hz bei 1 Mhz
	//TCCR1B |= (1<<CS10);	// Teiler 1 für ca. 3900 Hz bei 2 Mhz
	
}

void set_backlight(uint8_t pw){
	OCR3A = pw;
	if(pw > 0){
		LED_DDR |= (1<<LED);
	}else{
		LED_DDR &= ~(1<<LED);
	}
}
#if !K58B_POLL
ISR(INT6_vect){
	if(EICRB & INT6_RISING){
		if(k58b_low != 0){
			k58b_pw = (1000 * k58b_high) / (k58b_high + k58b_low);
			k58b_high = 0;
			k58b_low = k58b_timer;
			k58b_timer = 0;
			EICRB &= ~INT6_RISING;
			EICRB |= INT6_FALLING;
		}
	}else{
		k58b_low = 0;
		k58b_high = k58b_timer;
		k58b_timer = 0;
		EICRB &= ~INT6_RISING;
		EICRB |= INT6_RISING;
	}
	//return 0;
}

#endif

ISR(TIMER3_COMPA_vect){//100ns Timer for Pwm
	/*
	bel_tim++;
	if(bel_tim == 0){
		PORTE |= (1<<PE2);
	}else if(bel_tim == bel_pwm){
		PORTE &= ~(1<<PE2);
	}//else if(bel_tim == 40) bel_tim = 0;
	
// 	PORTE ^= (1<<PE2);
 	k58b_timer++;
 	if(k58b_timer == 0) k58b_pw = 100;
	 */
}