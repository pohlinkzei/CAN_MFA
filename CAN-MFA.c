/*
 * CAN_Sniffer.c
 *
 * Created: 14.03.2014 11:55:00
 *  Author: Hubert
 */ 
//*
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
//*/


#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "twi_slave.h"
#include "twi_task.h"
#include "can.h"
#include <string.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
//#include "font.h"
//#include "dogm-graphic.h"
#include "dog_display.h"
#include "dog_symbol.h"
#include "CAN-MFA.h"
#include "kl58b.h"
#include "calculation.h"
#include <avr/sleep.h>
#include "forum.h"

uint8_t frame_buffer[ROWS][COLUMNS] = {{0,}};
extern uint8_t reversed;
extern uint8_t underlined;
uint8_t get_text_length(char* text, uint8_t max_len);

uint8_t navi_old = 0xFF;

mfa_t mfa;
mfa_t mfa_old;

void adc_init(void);

void init_spi_lcd(void){
	DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3);
	SPCR = 0;
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1) | (0<<DORD) | (1<<CPOL) | (1<<CPHA);// | (1<<SPR1);// | (1<<SPR0);
	SPSR = (1<<SPI2X);
}

void avr_init(){
	ACSR |= (1<<ACD);
	PORTD |= (1<<PD1) | (1<<PD0);
	init_twi_slave(calculateID("MFA"));
	//timer 0 init
	// 1ms@8MHz
	TCCR0A = 0x00;
	TCCR0A |= (1<<WGM01) | (1<<CS01) | (1<<CS00); //ctc, prescaler 64
	OCR0A = F_CPU / 1000 / 64;//125; // 1ms (64*125=8000)
	TIMSK0 |= (1<<OCIE0A);
	// timer 1 init
	// 100ns@8MHz
	TCCR1B |= (1<<WGM12) | (1<<CS10); // (1<<CS11) ctc prescaler 8
	OCR1A = F_CPU / 10000 / 8;//100
	TIMSK1 = (1<<OCIE1A);
	// timer 3 init
	adc_init();
//*/	
	// PORTA:
	DDRA = 0x0F;
	MFA_SWITCH_PORT |= 0xF0;
	// PORTB
	DDRB = 0xE0;
	// PORTC
	DDRC = 0x3F;
	// PORTD
	DDRD = 0x1D;
	// PORTE
	DDRE = 0x3F;
	// PORTF
	DDRF = 0xF0;
	// PORTG
	DDRG = 0xFB;
	dog_spi_init();
	dog_init();
//	set_sleep_mode(SLEEP_MODE_IDLE);
//	sleep_enable();
	//#warning: "TODO: inits" done ;) 
}
	
void prepare_sleep(void){
	ADCSRA &= ~(1<<ADEN);
	TCCR1B = 0x00;
	TIMSK1 = 0x00;
	TCCR3B = 0x00;
	TIMSK3 = 0x00;
	TCCR0A = 0x00;
	TIMSK0 = 0x00;
	
}	

void recover_sleep(void){
	ADCSRA |= (1<<ADEN);
	TCCR0A = 0x00;
	TCCR0A |= (1<<WGM01) | (1<<CS01) | (1<<CS00); //ctc, prescaler 64
	OCR0A = F_CPU / 1000 / 64;//125; // 1ms (64*125=8000)
	TIMSK0 |= (1<<OCIE0A);
	// timer 1 init
	// 100ns@8MHz
	TCCR1B |= (1<<WGM12) | (1<<CS11); // ctc prescaler 8
	OCR1A = F_CPU / 10000 / 8;
	TIMSK1 = (1<<OCIE1A);

}	


int main(void){
	cli();
	avr_init();
	CAN_init(500,RX);
	can_status = 0;
	uint8_t a = 0;
	uint8_t b=0;
	DDRE |= (1<<PE2);
	PORTE |= (1<<PE2);
	char t4forum[] = "  www.t4forum.de  ";
	dog_home();
	dog_clear_lcd();					//0123456789012345678901
	dog_write_mid_string(NEW_POSITION(0,0), t4forum);
	for(a=0;a<6;a++){
		dog_set_position(a+2,4);
		for(b=0;b<128; b++){
			dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[a*128 + b])));
		}
	}
	uint64_t count = 0;
	sei();
	_delay_ms(1000);
	PORTE |= (1<<K15); // zündung an, bitte ;)
	//sei();
	line_shift_timer = 0;
	display_mode = SMALL_TEXT;
	display_value = CONS;
	mfa_mode = AVG;
	strcpy( (char*) radio_text, " Hocico - Ruptura ");
	speed[AVG] = 414;
	speed[CUR] = 1230;
	cons_l_100km[AVG] = 14.6;
	cons_l_100km[CUR] = 16.2;
	starterbat.integer = 14;
	starterbat.fraction = 42;
	zweitbat.integer = 14;
	zweitbat.fraction = 15;
	ambient_temperature = 24;
	oil_temperature = 103;
	driving_time[AVG] = 15864357;
	driving_time[CUR] = 123434;
	distance[AVG] = 100000;
	distance[CUR] = 13144757;
	range[AVG] = 0;	
	range[CUR] = 0;	
	engine_temperature = 87;
	rpm = 3260;
    while(1)
    {
		//dog_init();
		
		
		display_task();

		can_task();
		app_task();
		twi_task();
//		prepare_sleep();
//		sleep_mode();
//		recover_sleep();
		
    }
}



void save_message(CAN_message msg){
	//save message into sram
	if(msg.length == 8){// we probably received a valid message
		switch(msg.id){
			case ID280:{
				strcpy((char*) msg.data,(char*) id280_data);
				id280_valid = true;
				break;
			}
			case ID288:{
				strcpy((char*) msg.data,(char*) id288_data);
				id288_valid = true;
				break;
			}
			case ID380:{
				strcpy((char*) msg.data,(char*) id380_data);
				id380_valid = true;
				break;
			}
			case ID480:{
				cons_time[cons_cnt] = cons_timer; //get timing for consumption calculation
				strcpy((char*) msg.data,(char*) id480_data);
				id480_valid = true;
				break;
			}
			case ID320:{
				strcpy((char*) msg.data,(char*) id320_data);
				id320_valid = true;
				break;
			}
			case ID420:{
				strcpy((char*) msg.data,(char*) id420_data);
				id420_valid = true;
				break;
			}
		}
	}
}	

uint8_t get_text_length(char* text, uint8_t max_len){
	uint8_t len = 0;
	//uint8_t max_len = sizeof(text);
	for(len = 0; len < max_len; len++){
		if(text[len] == 0){
			//dog_set_position(6,10);
			//dog_write_small_string(text);
			//dog_set_position(7,10);
			//char str[3] = {0};
			//uint8_to_string(str, len);
			//dog_write_small_string(str);
			return len;
		}
	}
	return len;
}

void display_navi(void){
	if(navi_old == navigation_next_turn) return;
	uint8_t navi = 46;
	display_navigation_symbol48(NEW_POSITION(2,navi), navigation_next_turn);
	navi_old = navigation_next_turn;
}

void display_small_text(void){
	//uint8_t i;
	max_display_value = 1;
	// vier Werte pro screen
	//uint8_t left = 8; // position des rechten rands
	switch(display_value){
		case CAN_VALUES:{
			// speed, cons, rpm, water
			//				 "0123456789012345"
			char can_line2[16] = "                ";
			//				 "0123456789012345"
			char can_line3[16] = "                ";
						
			can_line2[11] = KMH;
			can_line2[12] = KMH + 1;
			can_line3[4] = ENGT;
			can_line3[5] = ENGT + 1;
			sprint_temperature(&can_line3[6],engine_temperature);
			can_line3[9] = 0xF8;
			can_line3[10] = 'C';
			//char line3[16] = {' ',  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,' ',' '};
							
			char can_line4[16] = "                ";//{' ', 13, 14, 15, 16,' ',' ',0xF8,'C',' ','m','i','n',0xB0,' ',' '};
			uint16_to_string(&can_line4[5], rpm);
			can_line4[10] = RPM;
			can_line4[11] = RPM + 1;
			//char line5[16] = "                ";//{' ',0xB1,' ',0xB2,' ',0,' ',' ' ,' ',' ',' ',' ',' ',' ',' ',' '};
			//				 "0123456789012345"
			char can_line5[16] = "                ";
			sprint_float(&can_line5[5],cons_l_100km[mfa_mode]);
			can_line5[10] = CONS;
			can_line5[11] = CONS + 1;
			can_line5[12] = CONS + 2;
						
			if(mfa_mode == CUR){
				can_line2[4] = ' ';
				can_line5[4] = ' ';
				sprint_cur_speed(&can_line2[7],speed[mfa_mode]);
			}else{
				can_line2[4] = 0x9D;
				can_line5[4] = 0x9D;
				sprint_avg_speed(&can_line2[6],speed[mfa_mode]);
			}
			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
						
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
						
			break;
		}
		case ADC_VALUES:{
			// spg1 spg2 oil aussentenp
						
			//				 "0123456789012345"
			//				 "   BB1 12,50V   "
			char adc_line2[16] = "                ";
			//				 "0123456789012345"
			//				 "   BB2 12,10V   "
			char adc_line3[16] = "                ";
			//				 "0123456789012345"
			//				 "    öT 103GC    "
			char adc_line4[16] = "                ";
			//				 "0123456789012345"
			//				 "    aT -03GC    "
			char adc_line5[16] = "                ";
			adc_line2[3] = BAT;
			adc_line2[4] = BAT+1;
			adc_line2[5] = '1';
			sprint_voltage(&adc_line2[6], starterbat);
			adc_line2[12] = 'V';
			adc_line3[3] = BAT;
			adc_line3[4] = BAT+1;
			adc_line3[5] = '2';
			sprint_voltage(&adc_line3[6], zweitbat);
			adc_line3[12] = 'V';
						
			adc_line4[4] = OILT;
			adc_line4[5] = OILT + 1;
			sprint_temperature(&adc_line4[7],oil_temperature);
			adc_line4[10] = 0xF8;
			adc_line4[11] = 'C';
			if(ambient_temperature < 30){
				adc_line5[4] = FROST;
				adc_line5[5] = FROST + 1;
			}
						
			sprint_temperature(&adc_line5[7],ambient_temperature);
			adc_line5[10] = 0xF8;
			adc_line5[11] = 'C';
			dog_write_mid_strings(NEW_POSITION(2,0),adc_line2, adc_line3);
						
			dog_write_mid_strings(NEW_POSITION(5,0),adc_line4, adc_line5);
			//*/
			break;
		}
	}		
}
//*
void display_med_text(void){
	uint8_t i;
	max_display_value = 9;
	char str[11] = "           ";
	switch(display_value){
		case VAL_SPEED:{//+VAL_CONS
			if(mfa_mode){
				sprint_cur_speed(&str[3], speed[CUR]);
			}else{
				str[1] = 0x9D;
				sprint_avg_speed(&str[2], speed[AVG]);
			}
			str[7] = KMH;
			str[8] = KMH+1;
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(2,i*12+6,4),str[i]);
				str[i] = ' ';
							
			}
			if(!mfa_mode){
				str[1] = 0x9D;
			}
			sprint_float(&str[2], cons_l_100km[mfa_mode]);
			str[7] = CONS;
			str[8] = CONS+1;
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(5,i*12+6,6),str[i]);
				str[i] = ' ';
							
			}			
			break;
		}
		//case VAL_VOLTB:
		case VAL_VOLTA:{//+
			dog_write_big_digit(NEW__POSITION(2,0,4),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(2,12,4),'1');
			//str[1] = BAT;
			uint8_t j = 0;
			sprint_voltage(&str[0], starterbat);
			//str[2] = BAT+1;
			//str[3] = '1';
			str[6] = 'V';
			//str[8] ='C';
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(2,36+i*12,4),str[i]);
				str[i] = ' ';
							
			}
			j=0;
			dog_write_big_digit(NEW__POSITION(5,0,6),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(5,12,6),'2');
			//str[1] = BAT;
			sprint_voltage(&str[0], zweitbat);
			//str[2] = BAT+1;
			//str[3] = '2';
			str[6] = 'V';
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(5,36+i*12,6),str[i]);
				str[i] = ' ';
							
			}
			break;
		}
					
		//case VAL_OIL:
		case VAL_WATER:{
			str[1] = OILT;
						
			sprint_temperature(&str[4], oil_temperature);
			str[2] = OILT+1;
			str[8] = 0xF8;
			str[9] = 'C';
			//str[8] ='C';
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(2,i*12,4),str[i]);
				str[i] = ' ';
							
			}
			str[1] = ENGT;
						
			sprint_temperature(&str[4], engine_temperature);
			str[2] = ENGT+1;
			str[8] = 0xF8;
			str[9] = 'C';
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(5,i*12,6),str[i]);
				str[i] = ' ';
							
			}
			break;
		}
		case VAL_EXTTEMP:{
			uint8_t j = 0;
			sprint_temperature(&str[4], ambient_temperature);
			
			str[8] = 0xF8;
			str[9] = 'C';
			//str[8] ='C';
			//dog_write_big_string(NEW__POSITION(2,0,6), speed_line);
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(2,i*12,4),str[i]);
				str[i] = ' ';			
			}
			
			if(ambient_temperature < 0){
				str[4] = FROST;
				str[5] = FROST +1;
				for(i=0; i<11; i++){
					dog_write_big_digit(NEW__POSITION(5,i*12,6),str[i]);
					str[i] = ' ';	
				}							
			}else{
				//*
				for(j=0; j<3; j++){
					dog_set_position(5+j,0);
					for(i=0; i<132; i++){
						dog_transmit_data(0x00);
					}
				}
			
				//*/
			}
			
			
			break;
		}		
		case VAL_TIME:{
			
			// "01234567890"
			// " 12:34:56  :
			//driving_time[mfa_mode]
			uint16_t year = (uint16_t) (driving_time[mfa_mode]/(3600*24*365));
			uint8_t day = (uint8_t) (driving_time[mfa_mode]/(3600*24))%365;
			uint8_t hour = (driving_time[mfa_mode]/3600)%24;
			uint8_t minute = (driving_time[mfa_mode]/60)%60; 
			uint8_t second = driving_time[mfa_mode]%60;
			
			
			// "01234567890"
			// "   12:364  "
			uint16_to_string(&str[4], day);
			
			uint8_to_string(&str[2], year);
			str[5] = 'y';
			str[9] = 'd';
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(2,i*12,7),str[i]);
				str[i] = ' ';			
			}
			uint8_to_string(&str[7], second);
			uint8_to_string(&str[4], minute);
			uint8_to_string(&str[1], hour);
			str[4] = 'h';
			str[7] = 'm';
			for(i=0; i<11; i++){
				dog_write_big_digit(NEW__POSITION(5,i*12,1),str[i]);
				str[i] = ' ';			
			}
			str[9] = 's';
			
			
			break;
		}				
		//case VAL_RPM:
		case VAL_MANIFOLD:{
			sprintf(str, " %lu ! ", (unsigned long) driving_time[mfa_mode]);
			dog_set_position(2,0);
			dog_write_small_string(str);
			break;
		}
		//case VAL_RANGE:
		case VAL_FUEL:{
			sprintf(str, "%lu ! ", (unsigned long) driving_time[mfa_mode]);
			dog_set_position(3,0);
			dog_write_small_string(str);
			break;
		}
		default:{
			display_value++;
			break;
		}			
	}
				
}
//*/
/*
void display_big_text(void){
	uint8_t i;
	max_display_value = 0;//9;
	switch(display_value){
		case VAL_SPEED:{
						
			break;
		}
		case VAL_CONS:{
			break;
		}
		case VAL_VOLTA:{
			break;
		}
		case VAL_VOLTB:{
			break;
		}
		case VAL_TIME:{
			break;
		}
		case VAL_OIL:{
			break;
		}
		case VAL_WATER:{
			break;
		}
		case VAL_RPM:{
			break;
		}
		case VAL_FUEL:{
			break;
		}
		case VAL_RANGE:{
			break;
		}
		default:{break;}
			
		
	}
		
	
	dog_set_position(2,0);
	//                      01234567890123456789012
	dog_write_small_string("01234567890123456789012");
	dog_set_position(3,0);
	dog_write_small_string("        BIG_TEXT       ");
	dog_set_position(4,0);
	dog_write_small_string("                       ");
	dog_set_position(5,0);
	dog_write_small_string("        BIG_TEXT       ");
	dog_set_position(6,0);
	dog_write_small_string("                       ");
	dog_set_position(7,0);
	dog_write_small_string("        BIG_TEXT       ");
}		
//*/
void display_tuer(void){
	max_display_value = 0;
	dog_set_position(2,0);
	//                      01234567890123456789012
	dog_write_small_string("01234567890123456789012");
	dog_set_position(3,0);
	dog_write_small_string("          TUER         ");
	dog_set_position(4,0);
	dog_write_small_string("                       ");
	dog_set_position(5,0);
	dog_write_small_string("          TUER         ");
	dog_set_position(6,0);
	dog_write_small_string("                       ");
	dog_set_position(7,0);
	dog_write_small_string("          TUER         ");
}

//

void display_can_data(void){
	//uint8_t i;
	max_display_value = 0;
	if(id280_valid || id288_valid || id380_valid || id480_valid || id320_valid || id420_valid){				
		uint8_t i;
		char id[2] = {0};
		dog_set_position(2,8);
		dog_write_tiny_string("280");
				
		for(i=0; i<8; i++){
			sprintf(id, "%02X", id280_data[i]);
			dog_transmit_data(0x00);
			dog_write_small_string(id);		
		}				
				
		dog_set_position(3,8);
		dog_write_tiny_string("288");
				
		for(i=0; i<8; i++){
			sprintf(id, "%02X", id288_data[i]);
					
			dog_transmit_data(0x00);
			dog_write_small_string(id);
		}	
				
		dog_set_position(4,8);
		dog_write_tiny_string("380");
				
		for(i=0; i<8; i++){
			sprintf(id, "%02X", id380_data[i]);
					
			dog_transmit_data(0x00);
			dog_write_small_string(id);
		}	
		dog_set_position(5,8);
		dog_write_tiny_string("480");
				
		for(i=0; i<8; i++){
			sprintf(id, "%02X", id480_data[i]);
					
			dog_transmit_data(0x00);
			dog_write_small_string(id);
		}	
				
		dog_set_position(6,8);
		dog_write_tiny_string("320");
				
		for(i=0; i<8; i++){
			sprintf(id, "%02X", id320_data[i]);
					
			dog_transmit_data(0x00);
			dog_write_small_string(id);
		}	
				
		dog_set_position(7,8);
		dog_write_tiny_string("420");
				
		for(i=0; i<8; i++){
			sprintf(id, "%02X", id420_data[i]);
					
			dog_transmit_data(0x00);
			dog_write_small_string(id);					
		}	
	}else{
		//cli();
		uint8_t a=0,b=0,c=2;
		for(a=0;a<6;a++){
			dog_set_position(a + c,4);
			for(b=0;b<128; b++){
				dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[a*128 + b])));
			}
		}
		//sei();
	}

}

void display_task(){
	/* 0123456789012345 *	* 012345678901234567890 *
	 - ---------------- -	* 
	 * Musiktitel (16)	*	*
	 - ---------------- -	*
	 *  Sym1 NAVI Sym2  *	*
	 *  VAL1 NAVI VAL2  *	*
	 *		 NAVI		*	*
	 *		 NAVI		*	*
	 --------------------	*
	 *						*
	 */
	//uint8_t i;
	/*	modes:
		NAVIGATION: Musiktitel (16 Zeichen, 8x12)
					Trennlinie 1x128 px
					Wert1 (2 Zeilen à 6 Zeichen, 8x12) Navipfeil (32x32px) Wert2 (2 Zeilen à 6 Zeichen, 8x12)
					Ankunftszeit Entfernung bis nä. Abzwg. Entfernung zum Ziel
					
		SMALL_TEXT: Musiktitel (16 Zeichen, 8x12)
					Trennlinie 1x128 px
	*/
	if(K15_PIN &  (1<<K15)){

		underlined = 1;
		dog_write_rotating(NEW_POSITION(0,10),(char*) radio_text, get_text_length((char*) radio_text, sizeof(radio_text)),SIZE8X12,line_shift_timer);
		underlined = 0;
		switch(display_mode){
			case NAVIGATION:{
				display_navi();
				break;
			}
			case SMALL_TEXT:{
				display_small_text();
				break;
			}
			case MED_TEXT:{
				display_med_text();
				break;
			}
			/*case BIG_TEXT:{
				display_big_text();
				break;
			}*/
			case CAN_DATA:{
				display_can_data();					
				break;
			}
		}	
		old_display_mode = display_mode;	
		#warning: "TODO: display and menue"
		
	}else if(TKML_PIN &  (1<<TKML)){
		display_mode = TUER;// print open door
		
	}else{
		display_mode = old_display_mode;
	}
	
	display_enable = 0;

}


void can_task(){
	if(K15_PIN & (1<<K15)){
		//read data
		if(id280_valid){
			// calculate rpm
			rpm = (id280_data[3] + ((id280_data[2]) << 8))>>2;
			id280_valid = false;
			can_status |= (1<<ID280);
		}
		if(id288_valid){
			engine_temperature = id288_data[1] - 100;
			speed[CUR] = ((id288_data[3] * 5) >> 2)*10;
			//speed = ((id288_data[3] * 50) + 5)/40;
			id288_valid = false;
			can_status |= (1<<ID288);
		}	
		if(id380_valid){
			id380_valid = false;
			can_status |= (1<<ID380);
		}
		if(id480_valid){
			cons_ul[cons_cnt] = id480_data[2] + ((id480_data[3] & 0x7F) << 8);
			if((cons_ul[cons_cnt] < cons_ul[cons_cnt-1])){
				cons_ul[cons_cnt] += 0x8000;
			}
			cons_cnt++;
			if (cons_cnt == 16){
				cons_timer = 0;
				uint32_t cons_ul_delta = cons_ul[15] - cons_ul[0];
				uint32_t cons_time_delta = cons_time[15] - cons_time[0];
				cons_l_h = ((cons_ul_delta * 3600.0) * cal_consumption)/256;
				cons_l_h /= (cons_time_delta * 1000.0);
				if (speed[CUR]>14){
					cons_l_100km[CUR] = (cons_l_h * 100.0)/speed[CUR];
				}			
				cons_cnt=0;
			}
			can_status |= (1<<ID480);
			id480_valid = false;
		}
		if(id320_valid){
			can_status |= (1<<ID320);
			id320_valid = false;
		}	
		if(id420_valid){
			can_status |= (1<<ID420);
			id420_valid = false;
		}
	}	
}

void adc_init(void){
	ADMUX |= (1<<REFS0);										// VCC with external capacitor at AREF
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC enabled, prescaler: 128
	ADCSRB = 0x00;
}
/***********************************************************************************************/
uint16_t read_adc(uint8_t portbit){
	uint8_t i;
	volatile uint16_t result;
	ADMUX |= portbit;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC)){;}
	result = ADCW; //dummy readout
	result = 0;
	for(i=0;i<4;i++){
		while(ADCSRA & (1<<ADSC)){;}
		result += ADCW;
	}
	ADMUX &= ~portbit;
	return (result>>2);
}

void reset_averages(void){
	;
}

void app_task(){;
	if(K15_PIN & (1<<K15)){
		//calculate averages 
		avg_cnt++;
		if(can_status & ID280 && can_status & ID288 && can_status & ID480){
			speed_sum += speed[CUR];
			cons_sum += (uint16_t) (10.0 * cons_l_100km[CUR]);
			cons_l_100km[AVG] = (float) (cons_sum / (10 * avg_cnt));
			speed[AVG] = speed_sum / avg_cnt;
			distance[AVG] = (uint32_t) ((speed[AVG] * driving_time[AVG]) / 3600);
			if(can_status & ID320 || can_status & ID420){// gauges are connected via can bus - read fuel and calculate range
				range[AVG] = (fuel * 100) / cons_l_100km[AVG]; 
				range[CUR] = (fuel * 100) / cons_l_100km[CUR];
			}
		}			
		//read analog inputs
		ADCSRA |= (1<<ADEN);
		uint16_t dummy = read_adc(0);
		dummy = dummy;
		starterbat = calculate_voltage(read_adc(SPANNUNG1));
		zweitbat = calculate_voltage(read_adc(SPANNUNG2));
		oil_temperature = calculate_oil_temperature(read_adc(OELTEMP));
		ambient_temperature = calculate_temperature(read_adc(AUSSENTEMP));
		if(!(MFA_SWITCH_PIN & (1<<MFA_SWITCH_MODE))){
			mfa.mode = true;
			if(mfa.mode != mfa_old.mode){
				mfa_mode = MODE1; //averages
			}				
		}else{
			mfa.mode = false;
			//if(mfa.mode != mfa_old.mode){
				mfa_mode = MODE2; //currents
			//}				
		}			
		if(!(MFA_SWITCH_PIN & (1<<MFA_SWITCH_RES))){
			mfa.res = true;
			if(mfa.res != mfa_old.res){
				display_value++;
				if(display_value > max_display_value){
					display_value = 0;
				}
				if(display_mode == MED_TEXT){
					display_values_big[mfa_mode]++;
					if(display_values_big[mfa_mode] > max_display_value){
						display_values_big[mfa_mode] = 0;
					}
				}else if(display_mode == SMALL_TEXT){
					display_values_small[mfa_mode]++;
					if(display_values_small[mfa_mode] > max_display_value){
						display_values_small[mfa_mode] = 0;
					}
				}
				
			}
			
			
				 
		// long: reset values by current display values ;)
			reset_averages();
		}else{
			mfa.res = false;
		}
		if(!(MFA_SWITCH_PIN & (1<<MFA_SWITCH_MFA))){
			mfa.mfa = true;
			if(mfa.mfa != mfa_old.mfa){
				display_mode++;
				if(display_mode > CAN_DATA){
					display_mode = 0;
				}
			}
		}else{
			mfa.mfa = false;
		}
		mfa_old = mfa;
	
	}	
}


ISR(TIMER0_COMP_vect){//1ms timer for consumption calculation
	cons_timer++;
	avg_timer++;
	line_ms_timer++;
	
	if(line_ms_timer > 100){
		display_enable = 1;
		line_ms_timer = 0;
		line_shift_timer++;
		//line_shift_timer++;
		//line_shift_timer++;
		//bel_pwm+=25;
	}
	if(avg_timer == 999){
		driving_time[CUR]++;
		driving_time[AVG]++;
		avg_timer = 0;
		line3timer++;
		if(line3timer>2){
			if(display_mode == NAVIGATION){
				navigation_next_turn++;
				if(navigation_next_turn>25){ 
					navigation_next_turn = 0;
				}					
				/*	display_mode++;
				//}
			}else{
				//*
				display_value++;
				if(display_value > max_display_value){
					display_value = 0;
					display_mode++;
					if(display_mode >5){
						display_mode = 0;
						mfa_mode ^= 0x01;
					}
				}	
				//*/										
			}			
		}//*/
	}
	
}
