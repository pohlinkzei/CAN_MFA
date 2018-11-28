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


volatile uint16_t k58b_timer = 0;
uint16_t volatile k58b_pw = 0;


void initk58_pwm(void){
	k58b_pw = 100;
	k58b_timer = 0; 
	
	TCCR1A = 0x00;
	TCCR1A |= (1<<COM1B1);	// Clear on match (nicht-invertierte PWM)
	TCCR1A |= (1<<WGM10);	// für WGM 1
		
	OCR1B = 0x00;
	TCNT1 = 0x00;
	TCCR1B = 0x00;
	//TCCR1B |= (1<<CS11);	// Teiler 8
	TCCR1B |= (1<<CS10) | (1<<CS11);	// Teiler 64 für ca. 60 Hz bei 2 Mhz
	//TCCR1B |= (1<<CS11);	// Teiler 8 für ca. 245 Hz bei 1 Mhz
	//TCCR1B |= (1<<CS10);	// Teiler 1 für ca. 3900 Hz bei 2 Mhz
	
}

void set_backlight(uint8_t pw){
	if(pw == OCR1B) return;
	OCR1B = pw;
	if(pw > 0){
		LED_DDR |= (1<<LED);
	}else{
		LED_DDR &= ~(1<<LED);
	}
}
