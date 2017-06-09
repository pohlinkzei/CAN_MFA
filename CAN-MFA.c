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
#include <avr/eeprom.h>
#include <string.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include "dog_display.h"
#include "dog_symbol.h"
#include "CAN-MFA.h"
#include "kl58b.h"
#include "calculation.h"
#include <avr/sleep.h>
#include "forum.h"
#include "tuer.h"
#include "uart_task.h"
#ifdef HAVE_MCP_ADC
#include "mcp3208.h"
#endif
	
#define FILTER_VALUE 3	
uint16_t adc_value[8] = {0,};
uint16_t old_adc_value[8] = {0,};	
extern volatile uint8_t reversed;
extern volatile uint8_t underlined;


uint8_t get_text_length(char* text, uint8_t max_len);
void display_tuer_open(void);
void display_tuer_closed(void);
uint8_t navi_old = 0x00;
volatile uint16_t timer2_cnt = 0;
mfa_t mfa;
mfa_t mfa_old;
volatile uint8_t cnt = 0;
volatile uint8_t k15_delay_cnt = 0;
volatile status_t status;
volatile status_t old_status;

volatile uint8_t id280_data[8];
volatile uint8_t id288_data[8];
volatile uint8_t id380_data[8];
volatile uint8_t id480_data[8];
volatile uint8_t id320_data[8];
volatile uint8_t id420_data[8];
volatile uint8_t id520_data[8];

volatile uint8_t id280_valid;
volatile uint8_t id288_valid;
volatile uint8_t id380_valid;
volatile uint8_t id480_valid;
volatile uint8_t id320_valid;
volatile uint8_t id420_valid;
volatile uint8_t id520_valid;

// values from can data
volatile int16_t speed[2]; //0-317km/h
volatile int16_t speed_start; //0-317km/h
volatile uint16_t rpm;	//0-16000rpm
volatile int16_t engine_temperature;//-100-154 centigrade
volatile uint8_t fuel;	//0-100% or 0-80l
//volatile uint8_t cons_cnt;
volatile uint16_t cons_delta_ul;
volatile uint16_t cons_delta_timer;
volatile float cons_l_h[2];
volatile float cons_l_h_start;
volatile float cons_l_100km[2];
volatile float cons_l_100km_start;
volatile uint32_t start_cnt = 0;
volatile int16_t gra_speed; //id288D4
volatile uint8_t pedal_position; //id280D6
volatile uint8_t eng_status0; //id280D1
volatile uint8_t eng_status1; //id288D2



#define MAX_AVG_CNT 1
uint8_t first_run = 1;
volatile float cons_avg[MAX_AVG_CNT];
volatile uint16_t speed_avg[MAX_AVG_CNT];

volatile uint64_t speed_sum = 15000;
volatile uint64_t speed_sum_start = 0;
volatile uint64_t cons_sum = 120;
volatile uint64_t cons_sum_start = 0;
volatile uint64_t avg_cnt = 50;
volatile uint64_t avg_cnt_start = 0;
volatile uint32_t driving_time[2];
volatile uint32_t driving_time_start;
volatile uint32_t distance[2];
volatile uint32_t distance_start;
volatile uint16_t range[2];

volatile voltage_value_t starterbat;
volatile voltage_value_t zweitbat;
volatile voltage_value_t v_solar_plus;
volatile voltage_value_t v_solar_minus;
volatile int16_t in_temperature;
volatile int16_t gearbox_temperature;
volatile int16_t ambient_temperature;
volatile int16_t oil_temperature;
uint8_t cal_water_temperature EEMEM;
uint8_t cal_voltage EEMEM;
uint8_t cal_speed EEMEM;
uint8_t cal_oil_temperature EEMEM;
uint8_t cal_consumption EEMEM;
uint8_t cal_gearbox_temperature EEMEM;
uint8_t cal_ambient_temperature EEMEM;
uint8_t cal_k15_delay EEMEM;
uint8_t cal_k58b_off_val EEMEM;
uint8_t cal_k58b_on_val EEMEM;
volatile uint16_t avg_timer;
extern volatile uint16_t k58b_timer;
volatile uint32_t cons_timer;
volatile uint8_t can_status = 0x00;

volatile uint8_t display_mode;
volatile uint8_t display_mode_tmp;
volatile uint8_t old_display_mode;
volatile uint8_t display_enable;
volatile uint8_t display_value;
volatile uint8_t display_value_top;
volatile uint8_t display_value_bot;
volatile uint8_t old_display_value;

volatile uint8_t navigation_next_turn;
volatile uint8_t navigation_status;
volatile uint8_t navigation_status_old;
volatile uint32_t distance_to_next_turn;

volatile uint8_t radio_text[AUDIO_STR_LENGTH];

volatile rx_t rx;
volatile tx_t tx;

volatile uint16_t line_ms_timer;
volatile uint16_t line_shift_timer;
volatile uint16_t line_wait_timer;
volatile uint16_t line3timer;
/*volatile uint16_t line4timer;
volatile uint16_t line5timer;
volatile uint16_t line6timer;
volatile uint16_t line7timer;
*/

volatile int16_t old_val = 0;
volatile int16_t new_val = 0;
uint8_t mfa_res_cnt = 0;
uint8_t no_res_switch = 0;

void adc_init(void);

void init_spi_lcd(void){
	DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3);
	SPCR = 0;
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1) | (0<<DORD) | (1<<CPOL) | (1<<CPHA);// | (1<<SPR1);// | (1<<SPR0);
	SPSR = (1<<SPI2X);
}

void reset_values(void){
	int i;
	for(i=0;i<8;i++){
		id280_data[i] = 0;
		id288_data[i] = 0;
		id380_data[i] = 0;
		id480_data[i] = 0;
		id320_data[i] = 0;
		id420_data[i] = 0;
		id520_data[i] = 0;
	}
	id280_valid = 0;
	id288_valid = 0;
	id380_valid = 0;
	id480_valid = 0;
	id320_valid = 0;
	id420_valid = 0;
	id520_valid = 0;

	// values from can data
	//0-317km/h
	speed[CUR] = 0; //0-317km/h
	speed[AVG] = 0;
	rpm = 0;	//0-16000rpm
	engine_temperature = 0;//-100-154 centigrade
	fuel = 0;	//0-100% or 0-80l
	cons_delta_ul = 0;
	cons_delta_timer = 0;
	cons_l_h[AVG] = 0;
	cons_l_h[CUR] = 0;
	cons_l_100km[CUR] = 0;
	cons_l_100km[AVG] = 0;

	speed_sum = 0;
	cons_sum = 0;
	avg_cnt = 0;
	driving_time[CUR] = 0;
	driving_time[AVG] = 0;
	distance[CUR] = 0;
	distance[AVG] = 0;
	range[CUR] = 0;
	range[AVG] = 0;
	// driving_time[AVG] = 0;//odo
	// driving_time[CUR] = 0;//trip
	// distance[AVG] = 0;//odo
	// distance[CUR] = 0;//trip
	// range[CUR] = 0;
	// range[AVG] = 0;

	starterbat.integer = 0;
	zweitbat.integer = 0;
	starterbat.fraction = 0;
	zweitbat.fraction = 0;
	ambient_temperature = 0;
	oil_temperature = 0;

	avg_timer = 0;
	k58b_timer = 0;
	cons_timer = 0;
	can_status = 0;
}

void timer0_init(void){
	
	TCCR0A = 0x00;
	//*
	// 1ms für alles mögliche
	TCCR0A |= (1<<WGM01) | (1<<CS01) | (1<<CS00); //ctc, prescaler 64
	
	#if K58B_POLL
	//0.1ms timer fuer pwm messung
	OCR0A = F_CPU / 64 / 10000; //250
	#else
	OCR0A = F_CPU / 64 / 1000; //250
	#endif
	TIMSK0 |= (1<<OCIE0A);
	//*/
}

void timer1_init(void){
	// 1s for consumption calculation
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS12);// ctc prescaler 1024
	OCR1A = F_CPU / 1024;
	TIMSK1 = (1<<OCIE1A);
}

void timer2_init(void){
	// timer 2 init
	// asy wakeup timer (0,25s)
	//return;
//	ACSR = 0x80;
//	ASSR  = (1<< AS2);
	_delay_ms(1000);
	TCCR2A = (1<<CS21) |(1<<CS20); //32
//	while((ASSR & (1<< TCR2UB)));
	TIFR2   &= ~(1<<OCIE2A);
	OCR2A = 32768 / 4 / 64;
	TIMSK2 |= (1<<OCIE2A);
	/*
	TCCR2A = 0x00;
	TCCR2A |= (1<<WGM21) | (1<<CS22); // prescaler 64
	OCR2A = F_CPU / 1000 / 64;
	TIMSK2 |= (1<<OCIE2A);
	*/
}

void io_init(void){
	// PORTD |= (1<<PD1) | (1<<PD0);
	// init_twi_slave(calculateID("MFA"));
	#if VERSION == 2
	// PORTA:
	DDRA = (1<<EN_ADC6) | (1<<EN_ADC7) /* | (1<<PA0) | (1<<PA1)*/;
	PORTA = 0x00;
	// PORTB
	DDRB = 0x00;
	PORTB = 0x00;
	// PORTC
	DDRC = (1<<EN3V3) /*| 0x1F*/;
	PORTC = 0x00;
	// PORTD
	DDRD = (1<<CS_DOG) | (1<<CAN_RS);
	PORTD = (1<<CS_DOG) | 3;
	// PORTE
	PORTE = 0x00;
	DDRE = (1<<EN_ADC0) | (1<<EN_ADC1);
	// PORTF
	DDRF = (1<<CS_ADC);
	PORTF |= (1<<CS_ADC);
	// PORTG
	DDRG = 0x00;
	PORTG = 0x00;
	#else
	DDRE |= (1<<PE2);
	#endif
	PCA_PORT |= (1<<DISABLE_PCA);
	PCA_DDR |= (1<<DISABLE_PCA);
}

void avr_init(){
	ACSR |= (1<<ACD);
	WDTCR &= (1<<WDE);
	ADCSRA = 0x00;
	
	io_init();
	
	timer0_init();
	timer1_init();

#if HAVE_MCP_ADC
	timer2_init();
	mcp3208_spi_init();
#else
	adc_init();
#endif
//*/	
	//can_init();
	uint8_t addr = calculateID("MFA");
	init_twi_slave(addr);
	
	dog_spi_init();
	dog_init();
	#if VERSION == 2
	uart_bootloader_init(UART_BAUD_RATE);
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	initk58_pwm();
	#else
	set_sleep_mode(SLEEP_MODE_IDLE);
	#endif
	sleep_enable();
	//#warning: "TODO: inits" done ;) 
}

status_t get_status(status_t old){
	status_t status = OFF;
	if(!(TKML_PIN & (1<<TKML))) status = DOOR_OPEN;
	if(K15_PIN & (1<<K15)) status = IGNITION_ON;
	
	if(old != status){
		switch(status){
			case DOOR_OPEN:{
#if VERSION == 2
				PORTC |= (1<<EN3V3);
				//k58b_pw = 100;
				dog_spi_init();
				initk58_pwm();	
#endif				
				LED_DDR |= (1<<LED);
				LED_PORT |= (1<<LED);
				dog_init();
				line_shift_timer = LINE_SHIFT_START;
				dog_clear_lcd();
				break;
			}
			case IGNITION_ON:{
				uint8_t a, b;
	#if VERSION == 2
				PORTE |= (1<<EN_ADC0) | (1<<EN_ADC1);
				PORTA |= (1<<EN_ADC6) | (1<<EN_ADC7);
				PORTC |= (1<<EN3V3);
				//k58b_pw = 100;
				dog_spi_init();
				initk58_pwm();	
	#endif			
				LED_DDR |= (1<<LED);
				LED_PORT |= (1<<LED);
				dog_init();
				char t4forum[] = "  www.t4forum.de  ";
				dog_home();
				dog_clear_lcd();					//0123456789012345678901
				underlined = 1;
				dog_write_mid_string(NEW_POSITION(0,0), t4forum);
				underlined = 0;
				for(a=0;a<6;a++){
					dog_set_position(a+2,4);
					for(b=0;b<128; b++){
						dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[a*128 + b])));
					}
				}
				PCA_PORT &= ~(1<<DISABLE_PCA);
				uint64_t count = 1000;
				while(count-- && (K15_PIN & (1<<K15))){
					_delay_ms(3);
				}
				line_shift_timer = LINE_SHIFT_START;
				dog_clear_lcd();
				break;
			}
			case OFF:{
				if(old == IGNITION_ON){
					k15_delay_cnt = eeprom_read_byte(&cal_k15_delay) + 1;
				}
				break;
			}
			default:{
				;
			}
		}
	}
	return status;
}

int main(void){
	status = OFF;
	cli();
	avr_init();
	can_init();	
	can_status = 0;
	
	sei();
	K15_PORT &= ~(1<<K15);
	//K15_PORT |= (1<<K15); // zündung an, bitte ;)
	line_shift_timer = LINE_SHIFT_START;
	display_mode = SMALL_TEXT;
	display_value = NAVIGATION;
	
	strcpy( (char*) radio_text, "  CAN Test        ");
	status = get_status(OFF);
	
	
	
    while(1)
    {
		status_t status_old = status;
		status = get_status(status_old);

		//*
		if(k58b_timer == 0){ // k58b off assuming bright ambient light -> value should be high(er)
			k58b_pw = eeprom_read_byte(&cal_k58b_off_val);
			//reversed = 1;
		}else{ // k58b on assuming dark ambient light -> value should be low(er)
			k58b_pw = eeprom_read_byte(&cal_k58b_on_val); 
			//reversed = 0;
		}
		//*/
		
		switch (status){
			case OFF:{
				if(old_status == DOOR_OPEN){
					display_tuer_closed();
					_delay_ms(1000);
				}
				int vtg_state = ((display_value == ADC_VALUES && display_mode == SMALL_TEXT) || (display_value == VAL_VOLTA && display_mode == MED_TEXT_TOP));
				int vtg_high = ((((starterbat.integer * 100) + starterbat.fraction) > 1280) || (zweitbat.fraction + (zweitbat.integer*100)) > 1280);
				if(vtg_state && vtg_high){
					app_task();
					display_task();
					k15_delay_cnt = 0;
					break;
				}else{
					if(k15_delay_cnt){
						app_task();
						display_task();
						break;
					}
					uint8_t i;
					k58b_pw = 0;
					// disable CAN receiver
					PCA_PORT |= (1<<DISABLE_PCA);
					dog_transmit(LCD_OFF);
#if VERSION == 2
					PORTE &= ~(1<<EN_ADC0) & ~(1<<EN_ADC1) & ~(1<<PE3);
					PORTA &= ~(1<<EN_ADC6) & ~(1<<EN_ADC7);
					PORTC &= ~(1<<EN3V3);
					DDRE &= ~(1<<PE3);
					TCCR3B = 0x00;
					TCCR3A = 0x00;
#else
					PORTE &= ~(1<<PE2);
#endif
					for(i=0;i<8;i++){
						id280_data[i] = 0;
						id288_data[i] = 0;
						id380_data[i] = 0;
						id480_data[i] = 0;
						id320_data[i] = 0;
						id420_data[i] = 0;
						id520_data[i] = 0;
					}
					id280_valid = 1;
				}
				sleep_mode();
				break;
			}
			case IGNITION_ON:{
				
				display_task();
				uart_bootloader_task();
				can_task();
				app_task();
				twi_task();
				break;
			}
			
			case DOOR_OPEN:{
				reversed = 0;
				k15_delay_cnt = 0;
				display_tuer_open();
				uart_bootloader_task();
				break;
			}
			default:{
				uint8_t a,b;
				char t4forum[] = "  www.t4forum.de  ";
				dog_home();
				dog_clear_lcd();					//0123456789012345678901
				underlined = 1;
				dog_write_mid_string(NEW_POSITION(0,0), t4forum);
				underlined = 0;
				for(a=0;a<6;a++){
					dog_set_position(a+2,4);
					for(b=0;b<128; b++){
						dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[a*128 + b])));
					}
				}
				break;
			}
		}
    }
}


uint8_t get_text_length(char* text, uint8_t max_len){
	uint8_t len = 0;
	for(len = 0; len < max_len; len++){
		if(text[len] == 0){
			return len;
		}
	}
	return len;
}

void display_navi(void){
	//dog_clear_lcd();
	//display_mode++;
	//return;
#if 1	//max_display_value = 26;
/*
	if(navi_old == navigation_next_turn){
		return;
		uint8_t a,b;
		for(a=0;a<6;a++){
			dog_set_position(a+2,4);
			for(b=0;b<128; b++){
				dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[(a+1)*128 - b])));
			}
		}
		//return;
	}
	if(navigation_next_turn != 4 || navigation_status)
	*/
	display_value = navigation_next_turn;
	uint8_t navi = 46;
	display_navigation_symbol48(NEW_POSITION(2,navi), navigation_next_turn);
	//navi_old = navigation_next_turn;
#else
	dog_set_page(2);
	char str[32] = {0,};
	// 0123456789012345678901234567890
	//" 0x00 0x00 0x00 0x00 0x00 0x00 " 	
	sprintf(str, " 0x%02X 0x%02X 0x%02X 0x%02X   ", rx.cal_consumption, rx.cal_oil_temperature, rx.cal_water_temperature, rx.cal_speed);
	dog_write_small_string(str);
	sprintf(str, " %u  ", (uint32_t) rx.distance_to_next_turn);  
	dog_set_page(3);
	dog_write_small_string(str);
	dog_set_page(4);
	dog_write_small_string(rx.radio_text);
	dog_set_page(5);
	sprint_distance(&str[2], rx.distance_to_next_turn);
	dog_write_small_string(str);
	dog_set_page(6);
	dog_write_small_string("                                     ");
	dog_set_page(7);
	dog_write_small_string("                                     ");
	
	

#endif
}

void display_small_text(void){
	//max_display_value = 2;
	// vier Werte pro screen
	switch(display_value){
		case CAN_VALUES:{
			// speed, cons, rpm, water
			//				 "0123456789012345"
			char can_line2[22] = "                ";
			//				 "0123456789012345"
			char can_line3[22] = "                ";
			char can_line4[22] = "                ";
			char can_line5[22] = "                ";
			
			#if 1			
			can_line2[11] = KMH;
			can_line2[12] = KMH + 1;
			can_line3[4] = ENGT;
			can_line3[5] = ENGT + 1;
			sprint_temperature(&can_line3[6],engine_temperature>25?engine_temperature:200);
			can_line3[9] = 0xF8;
			can_line3[10] = 'C';
			uint16_to_string(&can_line4[5], rpm);
			can_line4[10] = RPM;
			can_line4[11] = RPM + 1;
			if(mfa.mode == CUR && speed[CUR] < 100){
				sprint_float(&can_line5[5],cons_l_h[mfa.mode]);
				can_line5[11] = CONS_PER_HOUR;
				can_line5[10] = ' ';
				can_line5[12] = ' ';
				}else{
				sprint_float(&can_line5[5],cons_l_100km[mfa.mode]);
				can_line5[10] = CONS;
				can_line5[11] = CONS + 1;
				can_line5[12] = CONS + 2;
			}
			
			if(mfa.mode == CUR){
				can_line2[4] = ' ';
				can_line5[4] = ' ';
				sprint_cur_speed(&can_line2[7],speed[mfa.mode]);
				}else{
				can_line2[4] = 0x9D;
				can_line5[4] = 0x9D;
				sprint_avg_speed(&can_line2[6],speed[mfa.mode]);
			}
			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
			
			#else
			
			can_line2[6] = KMH;
			can_line2[7] = KMH + 1;
			if(mfa.mode == CUR){
				can_line2[0] = ' ';
				can_line5[0] = ' ';
				sprint_cur_speed(&can_line2[2],speed[mfa.mode]);
			}else{
				can_line2[0] = 0x9D;
				can_line5[0] = 0x9D;
				sprint_avg_speed(&can_line2[1],speed[mfa.mode]);
			}
			sprintf(&can_line2[9], "%06X ", speed_sum);
			
			if(mfa.mode == CUR && speed[CUR] < 100){
				sprint_float(&can_line5[1],cons_l_h[mfa.mode]);
				can_line5[7] = CONS_PER_HOUR;
				can_line5[6] = ' ';
				can_line5[8] = ' ';
			}else{
				sprint_float(&can_line5[1],cons_l_100km[mfa.mode]);
				can_line5[6] = CONS;
				can_line5[7] = CONS + 1;
				can_line5[8] = CONS + 2;
			}
			
			sprint_float(&can_line5[10], cons_l_h[mfa.mode]);
			can_line5[15] = CONS_PER_HOUR;
			
			can_line3[0] = ENGT;
			can_line3[1] = ENGT + 1;
			sprint_temperature(&can_line3[2],engine_temperature);
			can_line3[5] = 0xF8;
			can_line3[6] = 'C';
			
			uint16_to_string(&can_line3[9], rpm);
			can_line3[14] = RPM;
			can_line3[15] = RPM + 1;
			
			sprintf(can_line4, "v:%05d  %06X   ", old_val, cons_sum);
			
			//can_line4[0] = 'o';
			//int16_to_string(&can_line4[1], old_val);
			//can_line4[7] = 'n';
			//int16_to_string(&can_line4[8], new_val);
			//can_line4[10] = RPM;
			//can_line4[11] = RPM + 1;

			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
			
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
			#endif			
			break;
		}
		case CAN_VALUES2:{
			// status, gra,
			//				 "0123456789012345"
			char can_line2[22] = "                ";
			//				 "0123456789012345"
			char can_line3[22] = "                ";
			char can_line4[22] = "                ";
			char can_line5[22] = "                ";
			
			sprint_cur_speed(&can_line2[7],gra_speed * eeprom_read_byte(&cal_speed) * 10 / 128);
			can_line2[11] = KMH;
			can_line2[12] = KMH + 1;

			sprint_cur_speed(&can_line3[7],speed[CUR] );
			can_line3[11] = KMH;
			can_line3[12] = KMH + 1;
			
			can_line4[4] = 'L';
			sprint_temperature(&can_line4[6],pedal_position);
			can_line4[9] = '%'; //0xF8;

			sprintf(&can_line5[2], "0x%02X 0x%02X    ", eng_status0, eng_status1 );
			
			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
			
			break;
		}
		case ADC_VALUES:{
			
			if(starterbat.integer == 0 && zweitbat.integer == 0 && (oil_temperature > 300 || oil_temperature < -50) && (ambient_temperature > 200 || ambient_temperature <-50) && v_solar_plus.integer == 0 && v_solar_minus.integer == 0 && (gearbox_temperature > 300 || gearbox_temperature < -50) && (in_temperature > 200 || in_temperature <-50)){
				display_value++;
				break;
			}
			if(mfa.mode){
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
				if(ambient_temperature < AMBIENT_FROST_TEMP){
					adc_line5[4] = FROST;
					adc_line5[5] = FROST + 1;
				}
						
				sprint_temperature(&adc_line5[7],ambient_temperature);
				adc_line5[10] = 0xF8;
				adc_line5[11] = 'C';
				dog_write_mid_strings(NEW_POSITION(2,0),adc_line2, adc_line3);
						
				dog_write_mid_strings(NEW_POSITION(5,0),adc_line4, adc_line5);
				//*/
			}else{
				
				
				char adc_line2[16] = "                ";
				//				 "0123456789012345"
				//				 "   BB2 12,10V   "
				char adc_line3[16] = "                ";
				//				 "0123456789012345"
				//				 "    gT 103GC    "
				char adc_line4[16] = "                ";
				//				 "0123456789012345"
				//				 "    iT -03GC    "
				char adc_line5[16] = "                ";
				adc_line2[3] = BAT;
				adc_line2[4] = BAT+1;
				adc_line2[5] = '3';
				sprint_voltage(&adc_line2[6], v_solar_plus);
				adc_line2[12] = 'V';
				adc_line3[3] = BAT;
				adc_line3[4] = BAT+1;
				adc_line3[5] = '4';
				sprint_voltage(&adc_line3[6], v_solar_minus);
				adc_line3[12] = 'V';
				
				adc_line4[3] = GEARBOXT;
				adc_line4[4] = GEARBOXT + 1;
				adc_line4[5] = 'C';
				sprint_temperature(&adc_line4[7],gearbox_temperature);
				adc_line4[10] = 0xF8;
				adc_line4[11] = 'C';
				
				adc_line5[4] = INT;
				adc_line5[5] = INT + 1;
				sprint_temperature(&adc_line5[7],in_temperature);
				adc_line5[10] = 0xF8;
				adc_line5[11] = 'C';
				dog_write_mid_strings(NEW_POSITION(2,0),adc_line2, adc_line3);
				
				dog_write_mid_strings(NEW_POSITION(5,0),adc_line4, adc_line5);
			}
			break;
			}
			//*/
			case STANDARD_VALUES:{
				/*	0123456789012345
					RADIO TEXT
					----------------
					  ab reset/start
					 42kmh  12,3lkm
					  6,6lh  123gC
					 1234km  12:34
				*/
				char line1[17] = "                "; 
				char line2[17] = "                ";
				char line3[17] = "                ";
				char line4[17] = "                ";
				dog_set_position(2,0);
				//							"                ":"                ";
				strcpy(line1, mfa.mode==CUR?"    ab Reset    ":"    ab Start    ");
				
				sprint_cur_speed(&line2[1], mfa.mode==CUR?speed[AVG]:speed_start);
				line2[4] = KMH;
				line2[5] = KMH + 1;
				
				sprint_float(&line2[7], mfa.mode==CUR?cons_l_100km[AVG]:cons_l_100km_start);
				line2[12] = CONS;
				line2[13] = CONS + 1;
				line2[14] = CONS + 2;
				
				dog_write_mid_strings(NEW_POSITION(2,0), line1,line2);
				
				sprint_float(&line3[9], mfa.mode==CUR?cons_l_h[AVG]:cons_l_h_start);
				line3[14] = CONS_PER_HOUR;
	
				sprint_temperature(&line3[3],ambient_temperature);
				line3[6] = 0xF8;
				line3[7] = 'C';
				
				if(ambient_temperature < AMBIENT_FROST_TEMP){
					line3[1] = FROST;
					line3[2] = FROST + 1;
				}
				
				uint16_to_string(&line4[1], mfa.mode==CUR?distance[AVG]:distance_start);
				
				line4[6] = 'k';
				line4[7] = 'm';
				
				uint8_t dt_hour;
				uint8_t dt_minute;
				if(mfa.mode == CUR){
					dt_hour = (driving_time[AVG] / 3600);
					dt_minute = (driving_time[AVG] % 3600) / 60;
					
					uint8_to_string(&line4[9], dt_hour);
					uint8_to_string(&line4[12], dt_minute);
				}else{
					dt_hour = (driving_time_start / 3600);
					dt_minute = (driving_time_start % 3600) / 60;
					
					uint8_to_string(&line4[9], dt_hour);
					uint8_to_string(&line4[12], dt_minute);
				}
				
				line4[12] = ':';
				if(dt_hour < 10){
					line4[10] = '0';
				}
				if(dt_minute < 10){
					line4[13] = '0';
				}
				
				dog_write_mid_strings(NEW_POSITION(5,0), line3,line4);
				break;
			}
			#if 0
			case STANDARD_VALUES:{
				/*	0123456789012345
					RADIO TEXT
					----------------
					  ab reset/start
					 42kmh  12,3lkm
					  6,6lh  123gC
					 1234km  12:34
				*/
				char line1[17] = "                "; 
				char line2[17] = "                ";
				char line3[17] = "                ";
				char line4[17] = "                ";
				dog_set_position(2,0);
				//							"                ":"                ";
				strcpy(line1, mfa.mode==CUR?"    ab Reset    ":"    ab Start    ");
				
				sprint_cur_speed(&line2[1], mfa.mode==CUR?speed[AVG]:speed_start);
				line2[4] = KMH;
				line2[5] = KMH + 1;
				
				sprint_float(&line2[7], mfa.mode==CUR?cons_l_100km[AVG]:cons_l_100km_start);
				line2[12] = CONS;
				line2[13] = CONS + 1;
				line2[14] = CONS + 2;
				
				dog_write_mid_strings(NEW_POSITION(2,0), line1,line2);
				
				sprint_float(&line3[9], mfa.mode==CUR?cons_l_h[AVG]:cons_l_h_start);
				line3[14] = CONS_PER_HOUR;
	
				sprint_temperature(&line3[3],ambient_temperature);
				line3[6] = 0xF8;
				line3[7] = 'C';
				
				if(ambient_temperature < AMBIENT_FROST_TEMP){
					line3[1] = FROST;
					line3[2] = FROST + 1;
				}
				
				uint16_to_string(&line4[1], mfa.mode==CUR?distance[AVG]:distance_start);
				
				line4[6] = 'k';
				line4[7] = 'm';
				
				uint8_t dt_hour;
				uint8_t dt_minute;
				if(mfa.mode == CUR){
					dt_hour = (driving_time[AVG] / 3600);
					dt_minute = (driving_time[AVG] % 3600) / 60;
					
					uint8_to_string(&line4[9], dt_hour);
					uint8_to_string(&line4[12], dt_minute);
				}else{
					dt_hour = (driving_time_start / 3600);
					dt_minute = (driving_time_start % 3600) / 60;
					
					uint8_to_string(&line4[9], dt_hour);
					uint8_to_string(&line4[12], dt_minute);
				}
				
				line4[12] = ':';
				if(dt_hour < 10){
					line4[10] = '0';
				}
				if(dt_minute < 10){
					line4[13] = '0';
				}
				
				dog_write_mid_strings(NEW_POSITION(5,0), line3,line4);
				break;
			}
			#endif
			default:{
				display_value=0;
				break;
			}
		}
	}		

void display_med_row(uint8_t display_value, uint8_t page, uint8_t row){
	uint8_t i;
	char str[12] = "            ";
	switch(display_value){
		case VAL_SPEED:{
			sprint_cur_speed(&str[3], speed[CUR]);
			str[7] = KMH;
			str[8] = KMH+1;

			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_SPEED2:{
			str[1] = 0x9D;
			sprint_avg_speed(&str[2], speed[AVG]);
			str[7] = KMH;
			str[8] = KMH+1;
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_CONS:{
			if(speed[CUR] < 100){
				sprint_float(&str[2], cons_l_h[CUR]);
				str[7] = ' ';
				str[8] = CONS_PER_HOUR;
				}else{
				sprint_float(&str[2], cons_l_100km[mfa.mode]);
				str[7] = CONS;
				str[8] = CONS+1;
			}
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_CONS2:{
			str[1] = 0x9D;
			sprint_float(&str[2], cons_l_100km[mfa.mode]);
			str[7] = CONS;
			str[8] = CONS+1;
				
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		//
		case VAL_VOLTA:{//+
			if(starterbat.integer == 0){
				display_value++;
				break;
			}
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'1');
			sprint_voltage(&str[0], starterbat);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_VOLTB:{
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'2');
			sprint_voltage(&str[0], zweitbat);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_VOLTC:{
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'3');
			sprint_voltage(&str[0], v_solar_plus);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_VOLTD:{
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'4');
			sprint_voltage(&str[0], v_solar_minus);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_OIL:{
			if(oil_temperature > 300 && oil_temperature < -50){
				display_value++;
			}
			str[1] = OILT;
			
			sprint_temperature(&str[4], oil_temperature);
			str[2] = OILT+1;
			str[8] = 0xF8;
			str[9] = 'C';
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_ENGT:{
			str[1] = ENGT;
				
			sprint_temperature(&str[4], engine_temperature>25?engine_temperature:200);
			str[2] = ENGT+1;
			str[8] = 0xF8;
			str[9] = 'C';
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
					
			}
			break;
		}
		case VAL_AMBIENT:{
				
			if(ambient_temperature < 150 && ambient_temperature > -50){
				if(ambient_temperature < AMBIENT_FROST_TEMP){
					str[1] = FROST;
					str[2] = FROST +1;
				}
				sprint_temperature(&str[4], ambient_temperature);
				str[8] = 0xF8;
				str[9] = 'C';
				for(i=0; i<12; i++){
					dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
					str[i] = ' ';
				}
					
			}else{
				display_value++;
			}
			break;
		}
		case VAL_GEARBXT:{
			if(gearbox_temperature > 300 && gearbox_temperature < -50){
				display_value++;
			}
			
			str[1] = 'G';
			str[2] = 'B';
			
			sprint_temperature(&str[4], gearbox_temperature);

			str[8] = 0xF8;
			str[9] = 'C';

			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_INT:{
			if(in_temperature > 300 && in_temperature < -50){
				display_value++;
			}
			str[1] = 'i';
			str[2] = 'n';
			
			sprint_temperature(&str[4], in_temperature);

			str[8] = 0xF8;
			str[9] = 'C';

			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		//*
		case VAL_RPM:{
			//	0123456789012
			//	  12345 rpm
			uint16_to_string(&str[2], rpm);
			str[8] = RPM;
			str[9] = RPM + 1;
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}

		case VAL_TIME:{
			display_value++;
			break;	
			// "01234567890"
			// " 12:34:56  :
			//driving_time[mfa.mode]
			/*
			uint16_t year = (uint16_t) (driving_time[mfa.mode] / 31536000);//3600*24*365))));
			uint8_t day = (uint8_t) (driving_time[mfa.mode]/(86400)%365);
			uint8_t hour = (driving_time[mfa.mode]/3600)%24;
			uint8_t minute = (driving_time[mfa.mode]/60)%60;
			uint8_t second = driving_time[mfa.mode]%60;
				
				
			// "01234567890"
			// "   12:364  "
			uint16_to_string(&str[4], day);
				
			uint8_to_string(&str[2], year);
			str[5] = 'y';
			str[9] = 'd';
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			uint8_to_string(&str[7], second);
			uint8_to_string(&str[4], minute);
			uint8_to_string(&str[1], hour);
			str[4] = 'h';
			str[7] = 'm';
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(5,i*12,1),str[i]);
				str[i] = ' ';
			}
			str[9] = 's';
			//*/
			break;
		}
		case VAL_MANIFOLD:{
			display_value++;
			break;
			/*
			sprintf(str, " %lu ! ", (unsigned long) driving_time[mfa.mode]);
			dog_set_position(2,0);
			dog_write_small_string(str);
			dog_set_position(3,0);
			dog_write_small_string("Manifold");
			break;
			*/
		}

		case VAL_FUEL:{
			display_value++;
			/*
			sprintf(str, "%lu ! ", (unsigned long) driving_time[mfa.mode]);
			dog_set_position(3,0);
			dog_write_small_string(str);
			dog_set_position(4,0);
			dog_write_small_string("Fuel");
			*/
			break;
		}
		case VAL_RANGE:{
			display_value++;
			break;
		}
		//*/
		default:{
			display_value=0;
			break;
		}
	}

}

void display_med_text(void){
	
	/*
	upper half: displaymode = MED_TOP
	lower half: displaymode = MED_BOT
	*/
	if(display_mode == MED_TEXT_TOP){
		if(old_display_mode != MED_TEXT_TOP){
			display_value = display_value_top;
		}else{
			display_value_top = display_value;
		}
	}else if(display_mode == MED_TEXT_BOT){
		if(old_display_mode != MED_TEXT_BOT){
			display_value = display_value_bot;
		}else{
			display_value_bot = display_value;
		}
	}

	uint8_t page = 2;
	uint8_t row = 4;
	display_med_row(display_value_top, page, row);
	
	page = 5;
	row = 6;
	display_med_row(display_value_bot, page, row);
}
		
void display_tuer_open(void){
	uint8_t a,b;
	for(a=0;a<8;a++){
		dog_set_position(a,4);
		for(b=0;b<128; b++){
			dog_transmit_data(pgm_read_byte(&(sym_tuer2[a*128 + b])));
		}
	}
}

void display_tuer_closed(void){
	uint8_t a,b;
	for(a=0;a<8;a++){
		dog_set_position(a,4);
		for(b=0;b<128; b++){
			dog_transmit_data(pgm_read_byte(&(sym_tuer3[a*128 + b])));
		}
	}
}

void display_can_data(void){
	//return;
	//uint8_t i;
	//max_display_value = 1;
	//if(id280_valid || id288_valid || id380_valid || id480_valid || id320_valid || id420_valid || id520_valid){	
	if(can_status){
		#if 1
		if(mfa.mode){
			uint8_t i;
			char id[3] = {0,};
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
			uint8_t i;
			char id[2] = {0};
			dog_set_position(2,8);
			dog_write_tiny_string("520");
				
			for(i=0; i<8; i++){
				sprintf(id, "%02X", id520_data[i]);
				dog_transmit_data(0x00);
				dog_write_small_string(id);
			}
				
			dog_set_position(3,8);
			dog_write_tiny_string("xxx");
				
			for(i=0; i<8; i++){
				sprintf(id, "%02X", 255);
					
				dog_transmit_data(0x00);
				dog_write_small_string(id);
			}
				
			dog_set_position(4,8);
			dog_write_tiny_string("xxx");
				
			for(i=0; i<8; i++){
				sprintf(id, "%02X", 255);
					
				dog_transmit_data(0x00);
				dog_write_small_string(id);
			}
			dog_set_position(5,8);
			dog_write_tiny_string("xxx");
				
			for(i=0; i<8; i++){
				sprintf(id, "%02X", 255);
					
				dog_transmit_data(0x00);
				dog_write_small_string(id);
			}
				
			dog_set_position(6,8);
			dog_write_tiny_string("xxx");
				
			for(i=0; i<8; i++){
				sprintf(id, "%02X", 255);
					
				dog_transmit_data(0x00);
				dog_write_small_string(id);
			}
				
			dog_set_position(7,8);
			dog_write_tiny_string("xxx");
				
			for(i=0; i<8; i++){
				sprintf(id, "%02X", 255);
					
				dog_transmit_data(0x00);
				dog_write_small_string(id);
			}	
		}
		#else
		switch(display_value){
			case 0:{
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
				break;
			}
			case 1:{
				uint8_t i;
				char id[2] = {0};
				dog_set_position(2,8);
				dog_write_tiny_string("520");
				
				for(i=0; i<8; i++){
					sprintf(id, "%02X", id520_data[i]);
					dog_transmit_data(0x00);
					dog_write_small_string(id);
				}
				
				dog_set_position(3,8);
				dog_write_tiny_string("xxx");
				
				for(i=0; i<8; i++){
					sprintf(id, "%02X", 255);
					
					dog_transmit_data(0x00);
					dog_write_small_string(id);
				}
				
				dog_set_position(4,8);
				dog_write_tiny_string("xxx");
				
				for(i=0; i<8; i++){
					sprintf(id, "%02X", 255);
					
					dog_transmit_data(0x00);
					dog_write_small_string(id);
				}
				dog_set_position(5,8);
				dog_write_tiny_string("xxx");
				
				for(i=0; i<8; i++){
					sprintf(id, "%02X", 255);
					
					dog_transmit_data(0x00);
					dog_write_small_string(id);
				}
				
				dog_set_position(6,8);
				dog_write_tiny_string("xxx");
				
				for(i=0; i<8; i++){
					sprintf(id, "%02X", 255);
					
					dog_transmit_data(0x00);
					dog_write_small_string(id);
				}
				
				dog_set_position(7,8);
				dog_write_tiny_string("xxx");
				
				for(i=0; i<8; i++){
					sprintf(id, "%02X", 255);
					
					dog_transmit_data(0x00);
					dog_write_small_string(id);
				}
				break;
			}
			default:{
				display_value = 0;
				break;
			}
			
		}
#endif		
	}else{
		uint8_t a=0,b=0,c=2;
		for(a=0;a<6;a++){
			dog_set_position(a + c,4);
			for(b=0;b<128; b++){
				dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[a*128 + b])));
			}
		}
	}

}

void display_task(){
	/* 0123456789012345 *	* 012345678901234567890 *
	 - ---------------- -	* 
	 * Musiktitel (16)	*	*
	 - ---------------- -	*
	 * Fahrzeit/Strecke *	*
	 * Geschwindigkeit  *	*
	 *		Verbrauch	*	*
	 *		Temperatur	*	*
	 --------------------	*
	 *						*
	 */
	
	
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


	
	/*
	char first_line[18] = {0,};
	sprintf(first_line, " M:0x%02X V:0x%02X  ", display_mode, display_mode==NAVIGATION?navigation_next_turn:display_value);
	// " M:0x00 V:0x00 "
	// "0123456789012345
	dog_write_mid_string(NEW_POSITION(0,0),first_line);
	//*/
	/*
	if(strlen((char*) radio_text) < 16){
		uint8_t i;
		for(i=0; i<16; i++){
			if(radio_text[i] == 0){
				radio_text[i] = ' ';
			}
		}
	}
	//*/
	if(!(TKML_PIN & (1<<TKML))){
		//				.012345678901.
		#if 1
		char _str[] =	"            ";
		_str[4] = DOOR;
		_str[5] = DOOR + 1;
		_str[6] = DOOR2;
		dog_write_big_string(NEW_POSITION(0,4),_str);
		line_shift_timer = LINE_SHIFT_START;
		#else
		char _str[30];
		sprintf(_str, " nnt: %i, ns: %i ", navigation_next_turn, navigation_status);
		underlined = 1;
		dog_write_mid_string(NEW_POSITION(0,5), _str);
		underlined = 0;
		#endif
	}else{
		underlined = 1;
		dog_write_rotating(NEW_POSITION(0,5),(char*) radio_text, get_text_length((char*) radio_text, sizeof(radio_text)),SIZE8X12,line_shift_timer);
		underlined = 0;
	}
	
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
		case MED_TEXT_TOP:{
			display_med_text();
			break;
		}
		case MED_TEXT_BOT:{
			display_med_text();
			break;
		}
		case CAN_DATA:{
			display_can_data();					
			break;
		}
		default:{
			display_mode = 0;
			break;
		}
		
	}	
	old_display_mode = display_mode;	
	//#warning: "TODO: display and menue"
}



void adc_init(void){
	ADMUX |= (1<<REFS0);										// VCC with external capacitor at AREF
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC enabled, prescaler: 128
	ADCSRB = 0x00;
}
/***********************************************************************************************/
uint16_t read_adc(uint8_t portbit){
#ifdef HAVE_MCP_ADC
	uint16_t result = mcp3208_spi_read(SINGLE, portbit);
	return (result>>2);
#else
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
#endif
}

void read_adc_values(void){
	
	int i;
	adc_value[0] = read_adc(0);
	for(i=1; i<8;i++){
		//read all adc channels
		//old_adc_value[i] = adc_value[i];
		uint16_t temp = (adc_value[i]<<FILTER_VALUE) - adc_value[i];
		temp += read_adc(i);
		adc_value[i] = temp>>FILTER_VALUE;
	}
}

void reset_averages(void){
	avg_cnt = 0;
	speed_sum = 0;
	cons_sum = 0;
	cons_l_h[AVG] = 0;
	speed[AVG] = 0;
	distance[AVG] = 0;
	cons_l_100km[AVG] = 0;
	driving_time[AVG] = 0;

	avg_cnt_start = 0;
	speed_sum_start = 0;
	cons_sum_start = 0;
	cons_l_h_start = 0;
	speed_start = 0;
	distance_start = 0;
	cons_l_100km_start = 0;
	driving_time_start = 0;
	if(can_status & (1<<ID320) || can_status & (1<<ID420)){// gauges are connected via can bus - read fuel and calculate range
		range[AVG] = 0;
		range[CUR] = 0;
	}
}

void reset_averages_start(void){
	avg_cnt_start = 0;
	speed_sum_start = 0;
	cons_sum_start = 0;
	cons_l_h_start = 0;
	speed_start = 0;
	distance_start = 0;
	cons_l_100km_start = 0;
	driving_time_start = 0;
}

void enable_mfa_switch(void){
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_DDR |= (1<<MFA_SWITCH_GND);
	MFA_SWITCH_PORT |= (1<<MFA_SWITCH_MFA) | (1<<MFA_SWITCH_MODE) | (1<<MFA_SWITCH_RES);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_GND);
}

void disable_mfa_switch(void){
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_GND);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_GND);
}

void app_task(){
		enable_mfa_switch();
		read_adc_values();
		starterbat = calculate_voltage(adc_value[SPANNUNG1]);
		zweitbat = calculate_voltage(adc_value[SPANNUNG2]);
		
		v_solar_plus = calculate_voltage(adc_value[SPANNUNG3]);
		v_solar_minus = calculate_voltage(adc_value[SPANNUNG4]);
		gearbox_temperature = calculate_oil_temperature(adc_value[GETRIEBETEMP]);
		in_temperature = calculate_temperature(adc_value[INNENTEMP]);
		
		oil_temperature = calculate_oil_temperature(adc_value[OELTEMP]);
		ambient_temperature = calculate_temperature(adc_value[AUSSENTEMP]);
		
		//MFA_SWITCH_DDR |= (1<<MFA_SWITCH_GND);
		//#if 0
		if(!(MFA_SWITCH_PIN & (1<<MFA_SWITCH_MODE))){
			mfa.mode = CUR;
		}else{
			mfa.mode = AVG;
		}			
		//#endif
		if(!(MFA_SWITCH_PIN & (1<<MFA_SWITCH_RES))){
			//sprintf(radio_text, "%i           " ,mfa_res_cnt);
			mfa.res = 1;
			if(mfa.res == mfa_old.res){
				mfa_res_cnt++;
				if (mfa_res_cnt > 10){
					reset_averages_start();
					dog_clear_lcd();
					no_res_switch = 1;
					if(mfa_res_cnt > 25){
						// long: reset values by current display values ;)
						reset_averages();
						dog_set_lcd(0xFF);
						//no_res_switch = 1;
						mfa_res_cnt = 0;
					}
				}
			}
		}else{
			mfa.res = 0;
			mfa_res_cnt = 0;
			if(mfa.res != mfa_old.res){
				if(!no_res_switch){
					//if(display_value%2){
						display_value++;
					//}else{
					//	(mfa.mode==CUR)?mfa.mode=AVG:mfa.mode=CUR;
					//}
				}else{
					no_res_switch = 0;
				}
			}
		}
		if(!(MFA_SWITCH_PIN & (1<<MFA_SWITCH_MFA))){
			mfa.mfa = 1;
			if(mfa.mfa != mfa_old.mfa){
				display_mode++;
				if(display_mode > CAN_DATA){
					display_mode = 0;
				}
			}
		}else{
			mfa.mfa = 0;
		}
		mfa_old = mfa;
		disable_mfa_switch();
		//*
		// process navigation data
		if(navigation_status != navigation_status_old && (navigation_status == 4 || navigation_status == 3 || navigation_status == 3)){
			if(display_mode != NAVIGATION){
				display_mode_tmp = display_mode;
			}
			display_mode = NAVIGATION;
		}else if(display_mode == NAVIGATION){
			display_mode = display_mode_tmp;
		}
		
		//*/
}


ISR(TIMER0_COMP_vect){//1ms timer
	if(K58B_PIN & (1<<K58B)){
		k58b_timer=15;
	}else{
		if(k58b_timer > 0)
		k58b_timer--;
	}
	
	set_backlight(k58b_pw);
	line_ms_timer++;
	if(line_ms_timer > 400){
		display_enable = 1;
		line_ms_timer = 0;
		line_shift_timer += 5;
		if(line_shift_timer > 0xFFF) line_shift_timer = 0;
	}

}

ISR(TIMER1_COMPA_vect){
	
	if(status == IGNITION_ON){
		driving_time[CUR]++;
		driving_time[AVG]++;
		driving_time_start++;
		new_val = ((id480_data[3] & 0x7F) << 8) + id480_data[2];
		uint32_t delta = 0;
		if(new_val < old_val){
			delta = new_val - old_val + 0x7FFF;
			}else{
			delta = new_val - old_val;
		}
		old_val = new_val;
		calculate_consumption((uint16_t) delta, 1000);
		calculate_averages();
		old_val = new_val;
		start_cnt = 0;
	}else{
		if(k15_delay_cnt){
			k15_delay_cnt--;
		}
	}
	/*
	cnt++;
	if(cnt >10){
		K15_PORT ^= (1<<K15);
		cnt = 0;
	}
	//*/
}

ISR(TIMER2_COMP_vect){
	start_cnt++;
	if(start_cnt > 57600){ // 4h ohne k15
		reset_averages_start();
		start_cnt = 0;
	}
}