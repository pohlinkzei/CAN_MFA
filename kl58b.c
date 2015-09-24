/*
 * kl58b.c
 *
 * Created: 15.10.2014 10:13:55
 *  Author: Hubert
 # Frequenz K58B: 72,36Hz
 # Tastverhältnis: 20-100%
 */
#include "kl58b.h"

uint16_t volatile k58b_high;
uint16_t volatile k58b_low;
uint8_t volatile bel_pwm = 255;
uint8_t volatile bel_tim = 0;

void initk58_pwm(void){
	k58b_high = 0;
	k58b_low = 0;
	k58b_pw = 0;
	k58b_timer = 0; 
	EICRB |= INT6_RISING;  
	EIMSK |= (1<<INT6);
}

void set_k58_value(uint8_t value){
	//OCR3A = (value * k58b_scale) << 8;
	//bel
	;
}

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

ISR(TIMER1_COMPA_vect){//100ns Timer for Pwm
	bel_tim++;
	if(bel_tim == 0){
		PORTE |= (1<<PE2);
	}else if(bel_tim == bel_pwm){
		PORTE &= ~(1<<PE2);
	}//else if(bel_tim == 40) bel_tim = 0;
	
// 	PORTE ^= (1<<PE2);
 	k58b_timer++;
 	if(k58b_timer == 0) k58b_pw = 100;
}