/*
 * CAN-MFA.c
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
#include "navigation.h"
#include "display_task.h"
#include "kline.h"
#if HAVE_MCP_ADC
#include "mcp3208.h"
#endif
	
#define FILTER_VALUE 3	
uint16_t adc_value[8] = {0,};
uint16_t old_adc_value[8] = {0,};	
extern  uint8_t reversed;
extern  uint8_t underlined;

 uint8_t navi_old = 0x00;
volatile uint16_t timer2_cnt = 0;
 mfa_t mfa;
 mfa_t mfa_old;
 uint8_t cnt = 0;
volatile uint8_t k15_delay_cnt = 0;
volatile status_t status;
 status_t old_status;
 uint16_t door_delay = 0;
volatile uint16_t door_open_count = 0; 
volatile uint8_t id280_data[8];
volatile uint8_t id288_data[8];
volatile uint8_t id380_data[8];
volatile uint8_t id480_data[8];
volatile uint8_t id320_data[8];
volatile uint8_t id420_data[8];
volatile uint8_t id520_data[8];
volatile uint8_t id666_data[8];
volatile uint8_t id667_data[8];
 
 uint8_t id280_valid;
 uint8_t id288_valid;
 uint8_t id380_valid;
 uint8_t id480_valid;
 uint8_t id320_valid;
 uint8_t id420_valid;
 uint8_t id520_valid;
 uint8_t id666_valid;
 uint8_t id667_valid;

// values from can data
 int16_t speed[2]; //0-317km/h
int16_t max_speed;
 int16_t speed_start; //0-317km/h
volatile uint16_t rpm;	//0-16000rpm
 int16_t engine_temperature;//-48-143 centigrade
uint16_t max_rpm;
int16_t max_engine_temp;
int16_t min_engine_temp;
 uint8_t fuel;	//0-100% or 0-80l
//volatile uint8_t cons_cnt;
volatile uint16_t cons_delta_ul;
 uint16_t cons_delta_timer;
 float cons_l_h[2];
 float cons_km_l[2];
 float cons_km_l_start;
 float cons_l_h_start;
 float cons_l_100km[2];
 float cons_l_100km_start;
volatile uint32_t start_cnt = 0;
 int16_t gra_speed; //id288D4
 uint8_t pedal_position; //id280D6
 uint8_t eng_status0; //id280D1
 uint8_t eng_status1; //id288D2
 uint8_t do_not_switch_to_navigation;
 uint8_t button_irq = 0;

#define MAX_AVG_CNT 1
uint8_t first_run = 1;
 float cons_avg[MAX_AVG_CNT];
 uint16_t speed_avg[MAX_AVG_CNT];

volatile uint64_t speed_sum = 15000;
 uint64_t speed_sum_start = 0;
 uint64_t cons_sum = 120;
 uint64_t cons_sum_start = 0;
 uint64_t avg_cnt = 50;
 uint64_t avg_cnt_start = 0;
volatile uint32_t driving_time[2];
volatile uint32_t driving_time_start;
 uint32_t distance[2];
 uint32_t distance_start;
 uint16_t range[2];

 voltage_value_t starterbat;
 voltage_value_t zweitbat;
 voltage_value_t v_solar_plus;
 voltage_value_t entlastungsbat;
 int16_t in_temperature;
// int16_t max_in_temp;
// int16_t min_in_temp;
 int16_t gearbox_temperature;
int16_t max_gearbox_temp;
int16_t min_gearbox_temp;
 int16_t ambient_temperature;
int16_t max_ambient_temp;
int16_t min_ambient_temp;
 int16_t oil_temperature;
int16_t max_oil_temp;
int16_t min_oil_temp;
uint16_t manifold;
uint16_t min_manifold;
uint16_t max_manifold;
uint8_t EEMEM cal_i2c;
uint8_t EEMEM cal_ambient_temperature;
uint8_t EEMEM cal_voltage;
uint8_t EEMEM cal_speed;
uint8_t EEMEM cal_oil_temperature;
uint8_t EEMEM cal_manifold;
uint8_t EEMEM cal_consumption;
uint8_t EEMEM cal_gearbox_temperature;
uint8_t EEMEM cal_ambient_temperature;
uint8_t EEMEM cal_can_mode;
uint8_t EEMEM cal_startstop_enabled;
 uint8_t mkl;
uint8_t cal_k15_delay EEMEM;
uint8_t cal_k58b_off_val EEMEM;
uint8_t cal_k58b_on_val EEMEM;
 uint16_t avg_timer;
extern volatile uint16_t k58b_timer;
 uint32_t cons_timer;
volatile uint8_t can_status = 0x00;
 uint8_t engine_cut;
 uint8_t engine_cut_old;
 uint16_t draw_engine_cut_state;
 uint8_t startstop_enabled;
volatile uint8_t can_mode;

uint8_t display_mode = 0;
uint8_t display_mode_tmp;
uint8_t old_display_mode;
volatile uint8_t display_enable;
volatile uint8_t display_enable_cnt;
volatile uint8_t t0cnt = 0;
 uint16_t tmp_rpm;
volatile uint16_t rpm_cnt;
volatile uint16_t hg_cnt;
volatile uint16_t cons_cnt;
 uint8_t display_value[8] = {0, STANDARD_VALUES, VAL_AMBIENT, VAL_CUR_CONS, RADIO_TEXT,0,0,0}; // navi, small, med_top, med_bot, can, top_line, settings, invalid
 uint8_t display_value_top = VAL_AMBIENT;
 uint8_t display_value_bot = VAL_CUR_CONS;
 uint8_t old_display_value[8] = {0,0,0,0,0,0,0,0};

 uint8_t navigation_next_turn;
 uint8_t navigation_status;
 uint8_t navigation_status_old;
 uint32_t distance_to_next_turn;

 uint8_t radio_text[AUDIO_STR_LENGTH];

 rx_t rx;
 tx_t tx;

volatile uint16_t line_ms_timer;
volatile uint16_t line_shift_timer;
volatile uint16_t line_wait_timer;
volatile uint16_t line3timer;
/*volatile uint16_t line4timer;
volatile uint16_t line5timer;
volatile uint16_t line6timer;
volatile uint16_t line7timer;
*/
volatile uint8_t send_can_message;
volatile uint8_t send_can_lock;

volatile int16_t old_val = 0;
volatile int16_t new_val = 0;
uint16_t mfa_res_cnt = 0;
uint8_t no_res_switch = 0;
uint16_t mfa_mfa_cnt = 0;
uint8_t no_mfa_switch = 0;

uint16_t t3cnt = 0;

void adc_init(void);

void init_spi_lcd(void){
	DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3);
	SPCR = 0;
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1) | (0<<DORD) | (1<<CPOL) | (1<<CPHA);// | (1<<SPR1);// | (1<<SPR0);
	SPSR = (1<<SPI2X);
}

void enable_mfa_switch(void){
#if MFA_BUTTONS_ACTIVE_LOW
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_DDR |= (1<<MFA_SWITCH_GND);
	MFA_SWITCH_PORT |= (1<<MFA_SWITCH_MFA) | (1<<MFA_SWITCH_MODE) | (1<<MFA_SWITCH_RES);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_GND);
#else
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_DDR |= (1<<MFA_SWITCH_GND);
	MFA_SWITCH_PORT |= (1<<MFA_SWITCH_GND);
#endif
}

void disable_mfa_switch(void){
	//return;
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_DDR &= ~(1<<MFA_SWITCH_GND);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_MFA) & ~(1<<MFA_SWITCH_MODE) & ~(1<<MFA_SWITCH_RES);
	MFA_SWITCH_PORT &= ~(1<<MFA_SWITCH_GND);
}

uint8_t read_mfa_switch(uint8_t button){
#if MFA_BUTTONS_ACTIVE_LOW
	return !(MFA_SWITCH_PIN & (1<<button));
#else
	return MFA_SWITCH_PIN & (1<<button);
#endif
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
	engine_temperature = 0;//-48-143 centigrade
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
	// 0.1ms für alles mögliche
	TCCR0A |= (1<<WGM01) | (1<<CS01);// prescaler 8 | (1<<CS00); //ctc, prescaler 64

	OCR0A = F_CPU / 8 / 10000; //200

	TIMSK0 = (1<<OCIE0A);
	//*/
}

void timer3_init(void){
	// 1s for consumption calculation
	TCCR3B |= (1<<WGM32) | (1<<CS30) | (1<<CS32);// ctc prescaler 1024
	OCR3A = F_CPU / 1024;
	TIMSK3 = (1<<OCIE3A);
}

void timer2_init(void){
	// timer 2 init
	// asy wakeup timer (0,25s)
	//return;
	ACSR = 0x80;
	ASSR  = (1<< AS2);
	_delay_ms(1000);
	TCCR2A = (1<<CS21) |(1<<CS20); //32
	//while((ASSR & (1<< TCR2UB)));
	TIFR2   &= ~(1<<OCIE2A);
	OCR2A = 32768 / 4 / 64;
	TIMSK2 = (1<<OCIE2A);
}

void io_init(void){
	// PORTA:
	DDRA = 0x0F;
	PORTA = 0x00;
	// PORTB
	DDRB = 0x88;
	PORTB = 0x00;
	// PORTC
	DDRC = 0xB2 /*| 0x1F*/;
	PORTC = 0x00;
	// PORTD
	DDRD = (1<<CAN_RS);
	PORTD = (1<<SCL) | (1<<SDA);
	// PORTE
	PORTE = 0x00;
	DDRE = 0x00;
	// PORTF
	DDRF = 0x00;
	PORTF |= 0x00;
	// PORTG
	DDRG = 0x07;
	PORTG = 0x00;

	PCA_PORT |= (1<<DISABLE_PCA);
	PCA_DDR |= (1<<DISABLE_PCA);
}

void avr_init(){
	ACSR |= (1<<ACD);
	//WDTCR |= (1<<WDE);
	ADCSRA = 0x00;
	
	io_init();
	
	timer0_init();
	timer3_init();
	timer2_init();
#if HAVE_MCP_ADC
	mcp3208_spi_init();
#else
	adc_init();
#endif

	if(eeprom_read_byte(&cal_can_mode) == NO_CAN){
		can_mode = NO_CAN;
		can_init_nocan();
		can_status = 0;
		//init int 0/1 all/rising edge
		//rpm @ int5 -> int on toggle, cons @ int6 on rising edge, HG @ int7 (rising edge)

		EICRB =  (1<<ISC50) | (1<<ISC61) | (1<<ISC60) | (1<<ISC71) | (1<<ISC70);
		//enable
		EIMSK = (1<<INT5) | (1<<INT6) | (1<<INT7);
		
	}else{
		can_mode = CAN;
		can_init();
		can_status = 0;
		if(eeprom_read_byte(&cal_can_mode) == NO_I2C){
			uint8_t addr = calculateID("MFA");
			init_twi_slave(addr);
		}
		
	}

	dog_spi_init();
	dog_init();
	
	uart_bootloader_init(UART_BAUD_RATE);
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	initk58_pwm();

	kline_uart_init(9600);
	kline_io_init();

	sleep_enable();
	//#warning: "TODO: inits" done ;) 
}

status_t get_status(status_t old){
	status_t status = OFF;
	if(!(TKML_PIN & (1<<TKML))) status = DOOR_OPEN;
	if(K15_PIN & (1<<K15) || (button_irq?button_irq--:button_irq)) status = IGNITION_ON;

	if(old != status){
		switch(status){
			case DOOR_OPEN:{

				//k58b_pw = 100;
				dog_spi_init();
				initk58_pwm();	
			
				LED_DDR |= (1<<LED);
				LED_PORT |= (1<<LED);
				dog_init();
				line_shift_timer = LINE_SHIFT_START;
				dog_clear_lcd();
				break;
			}
			case IGNITION_ON:{
				uint8_t a, b;
				//k58b_pw = 100;
				dog_spi_init();
				initk58_pwm();	
		
				LED_DDR |= (1<<LED);
				LED_PORT |= (1<<LED);
				dog_init();
				if(button_irq == 0){
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
				}
				line_shift_timer = LINE_SHIFT_START;
				dog_clear_lcd();
				break;
			}
			case OFF:{
				if(old == IGNITION_ON){
					k15_delay_cnt = eeprom_read_byte(&cal_k15_delay) + 1;
				}
				if(old == DOOR_OPEN){
					door_delay = 1000; //ms
				}
				break;
			}
			default:{
				;
			}
		}
	}/*else{
		if(status==DOOR_OPEN){
			door_open_count++;
		}
	}*/
	return status;
}

int main(void){
	status = OFF;
	cli();
	avr_init();
	
	sei();
	
	K15_PORT &= ~(1<<K15);
	
	line_shift_timer = LINE_SHIFT_START;
	#if 1
	display_mode = SMALL_TEXT;
	display_value[SMALL_TEXT] = STANDARD_VALUES;
	#else
	TKML_PORT |= (1<<TKML);
	K15_PORT |= (1<<K15); // zündung an, bitte ;)
	display_mode = SMALL_TEXT;
	display_value[SMALL_TEXT] = STANDARD_VALUES;
	#endif
	//strcpy( (char*) radio_text, "  CAN Test        ");

	#if ENABLE_SETTINGS
	display_menu_init();
	#endif
	status = get_status(OFF);
	
	enable_mfa_switch();

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
		#if 1 
		switch (status){
			case DOOR_OPEN:{
				reversed = 0;
				k15_delay_cnt = 0;
				if(door_open_count < 300){
					display_tuer_open();
					uart_bootloader_task();
					break;
				}
				
			}
			case OFF:{
				int vtg_state = ((display_value[SMALL_TEXT] == ADC_VALUES && display_mode == SMALL_TEXT)
							 || (display_value[MED_TEXT_TOP] == VAL_VOLTA && display_mode == MED_TEXT_TOP)
							 || (display_value[MED_TEXT_TOP] == VAL_VOLTB && display_mode == MED_TEXT_TOP)
							 || (display_value[MED_TEXT_TOP] == VAL_VOLTC && display_mode == MED_TEXT_TOP)
							 || (display_value[MED_TEXT_TOP] == VAL_VOLTD && display_mode == MED_TEXT_TOP)
							 || (display_value[MED_TEXT_BOT] == VAL_VOLTA && display_mode == MED_TEXT_BOT)
							 || (display_value[MED_TEXT_BOT] == VAL_VOLTB && display_mode == MED_TEXT_BOT)
							 || (display_value[MED_TEXT_BOT] == VAL_VOLTC && display_mode == MED_TEXT_BOT)
							 || (display_value[MED_TEXT_BOT] == VAL_VOLTD && display_mode == MED_TEXT_BOT)
							 );
				int vtg_high = ((((starterbat.integer * 100) + starterbat.fraction) > 1280) || (zweitbat.fraction + (zweitbat.integer*100)) > 1280);
				if(vtg_state && vtg_high){
					if(display_enable){
						display_enable = 0;
						app_task();
						display_task();
					}
					k15_delay_cnt = 0;
					break;
				}else{
					if(k15_delay_cnt && display_enable){
						display_enable = 0;
						app_task();
						display_task();
						break;
					}
					//*
					if(door_delay){
						display_tuer_closed();
						break;
					}

					if(read_mfa_switch(MFA_SWITCH_RES) || read_mfa_switch(MFA_SWITCH_MFA)){
						_delay_ms(300);
						button_irq = 255;
						break;
					}
					//*/
					uint8_t i;
					// disable CAN receiver
					PCA_PORT |= (1<<DISABLE_PCA);
					dog_disable();

					for(i=0;i<8;i++){
						id280_data[i] = 0;
						id288_data[i] = 0;
						id380_data[i] = 0;
						id480_data[i] = 0;
						id320_data[i] = 0;
						id420_data[i] = 0;
						id520_data[i] = 0;
						id666_data[i] = 0;
						id667_data[i] = 0;
					}
					id280_valid = 1;
				}
				set_backlight(0);
				sleep_mode();
				break;
			}
			case IGNITION_ON:{
				door_delay = 0;
				if(display_enable){
					display_enable = 0;
					display_task();
					uart_bootloader_task();
					app_task();
					if (eeprom_read_byte(&cal_i2c) == NO_I2C){
						twi_task();
					}
				}
				can_task();
				
				//kline_task();
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
	#endif
    }
	
}

void adc_init(void){
	ADMUX |= (1<<REFS0);										// VCC with external capacitor at AREF
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC enabled, prescaler: 128
	ADCSRB = 0x00;
}
/***********************************************************************************************/
uint16_t read_adc(uint8_t portbit){
#if HAVE_MCP_ADC
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

void reset_min_max_values(void){
	//speed, rpm, temperature (eng, oil, out, gaerbox)
	max_gearbox_temp= -50;
	//max_in_temp=-50;
	max_oil_temp=-50;
	max_ambient_temp = -50;
	max_speed= 0;
	max_rpm = 0;
	min_gearbox_temp= 150;
	//min_in_temp = 150;
	min_oil_temp = 150;
	min_ambient_temp = 150;
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


void switch_task(void){
	enable_mfa_switch();
	if(read_mfa_switch(MFA_SWITCH_MODE)){
		mfa.mode = CUR;
	}else{
		mfa.mode = AVG;
		#if ENABLE_SETTINGS
		if((display_mode & (1<<SETTINGS))){
			// used to save changed values to eeprom
			display_settings_save_value();
		}
		#endif
	}			

	if(read_mfa_switch(MFA_SWITCH_RES)){
		mfa.res = 1;
		if(mfa.res == mfa_old.res){
			mfa_res_cnt++;
			#if ENABLE_SETTINGS
			if(display_mode & (1<<SETTINGS)){
				if(current_entry->parent == NULL){
					if (mfa_res_cnt > 10){
						dog_clear_lcd();
						no_res_switch = 1;
						if(mfa_res_cnt > 25){
							void (*reset)( void ) = 0xF800;
							reset();
						}
					}
				}
			}else	
			#endif
			{
				if (mfa_res_cnt > 10){
					if(display_mode == SMALL_TEXT && display_value[SMALL_TEXT] == MIN_MAX_VALUES){
						reset_min_max_values();
						mfa_res_cnt = 0;
					}else{
						reset_averages_start();
					}
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
		}
	}else{
		mfa.res = 0;
		mfa_res_cnt = 0;
		if(mfa.res != mfa_old.res){
			if(!no_res_switch){
				#if ENABLE_SETTINGS
				if(display_mode & (1<<SETTINGS)){
					field_position++;
					if(field_position > max_field_position){
						field_position = 0;
					}
				}else
				#endif
				{
					display_value[display_mode]++;
				}
			}else{
				no_res_switch = 0;
			}
		}
	}
	if(read_mfa_switch(MFA_SWITCH_MFA)){
		mfa.mfa = 1;
		#if ENABLE_SETTINGS
		if(mfa.mfa == mfa_old.mfa){
			mfa_mfa_cnt++;
			if(mfa_mfa_cnt>25){
				if(display_mode & (1<<SETTINGS)){
					display_mode &= ~(1<<SETTINGS);
				}else{
					display_mode |= (1<<SETTINGS);
					current_entry = (menu_item_t*) &settings_menu;
				}
				mfa_mfa_cnt = 0;
				no_mfa_switch = 1;
			}
		}
		#endif
	}else{
		mfa.mfa = 0;
		mfa_mfa_cnt = 0;
		if(mfa.mfa != mfa_old.mfa){
			if(!no_mfa_switch){
				#if ENABLE_SETTINGS
				if(display_mode & (1<<SETTINGS)){
					// Settings display
					if(field_position == 0){
						current_entry = (menu_item_t*)(current_entry->parent?current_entry->parent:&settings_menu);
					}else{
						if(current_entry->is_value){
							switch(field_position){
								case 1: current_entry->value += 100; break;
								case 2: current_entry->value += 10; break;
								case 3: current_entry->value += 1; break;
								case 4: current_entry->value -= 100; break;
								case 5: current_entry->value -= 10; break;
								case 6: current_entry->value -= 1; break;
							}
						}else if (current_entry->is_switch){
							switch(field_position){
								case 1: current_entry->switch_value = 1; break;
								case 2: current_entry->switch_value = 0; break;
							}
						}else{
							current_entry = display_settings_nth_child((menu_item_t*) current_entry, field_position-1);
							field_position = 0;
						}
					}
				}else
				#endif
				{
					display_mode++;
				}
			}else{
				no_mfa_switch = 0;
			}
			if((navigation_status == status_routing || navigation_status == status_recalculating) && display_mode == NAVIGATION){
				do_not_switch_to_navigation = 1;
			}else{
				do_not_switch_to_navigation = 0;
			}
		}
	}
	mfa_old = mfa;
	disable_mfa_switch();
}

void app_task(){
		read_adc_values();
		starterbat = calculate_voltage(adc_value[SPANNUNG1]);
		zweitbat = calculate_voltage(adc_value[SPANNUNG2]);
		if(can_mode == NO_CAN){
			//voltage_value_t v_mkl = calculate_voltage(adc_value[MKL_NOCAN]);
			//mkl = (uint8_t) (v_mkl.integer < 8); // mkl is active low!
			mkl = (MKL_PIN & (1<<MKL));

			// TODO: Calculate RPM
			if(rpm_cnt < 3001)
				rpm = (uint16_t) ((uint32_t) (600000 / tmp_rpm));
			else
				rpm = 0;
			rpm_cnt = 0;
				// TODO: Set can send timing dependent (10ms / 100ms / ...)
			


		}else{
			v_solar_plus = calculate_voltage(adc_value[SPANNUNG3]);
			entlastungsbat = calculate_voltage(adc_value[SPANNUNG4]);

			if(engine_temperature > -45 && engine_temperature < 142){
				if(max_engine_temp < engine_temperature){
					max_engine_temp = engine_temperature;
					}else if(min_engine_temp > engine_temperature){
					min_engine_temp = engine_temperature;
				}
			}

			gearbox_temperature = calculate_gearbox_temperature(adc_value[GETRIEBETEMP]);
			if(gearbox_temperature < 150 && gearbox_temperature > -50){
				if(max_gearbox_temp < gearbox_temperature){
					max_gearbox_temp = gearbox_temperature;
				}else if(min_gearbox_temp > gearbox_temperature){
					min_gearbox_temp = gearbox_temperature;
				}
			}

			oil_temperature = calculate_oil_temperature(adc_value[OELTEMP]);
			if(oil_temperature < 150 && oil_temperature > -50){
				if(max_oil_temp < oil_temperature){
					max_oil_temp = oil_temperature;
				}else if(min_oil_temp > oil_temperature){
					min_oil_temp = oil_temperature;
				}
			}

			manifold = calculate_manifold(adc_value[MANIFOLD]);
			if(manifold < 2700 && manifold > 20){
				if(max_manifold < manifold){
					max_manifold = manifold;
				}else if(min_manifold > manifold){
					min_manifold = manifold;
				}
			}
		}
		
		ambient_temperature = calculate_ambient_temperature(adc_value[AUSSENTEMP]);
		if(ambient_temperature < 150 && ambient_temperature > -50){
			if(max_ambient_temp < ambient_temperature){
				max_ambient_temp = ambient_temperature;
				}else if(min_ambient_temp > ambient_temperature){
				min_ambient_temp = ambient_temperature;
			}
		}

		if(max_speed < speed[CUR]){
			max_speed = speed[CUR];
		}

		if(max_rpm < rpm){
			max_rpm = rpm;
		}
		
		switch_task();

		if(engine_cut != engine_cut_old){
			// new status from startstop device
			draw_engine_cut_state = 5000;
			engine_cut_old = engine_cut;
		}
		//*
		// process navigation data
		
		if(!do_not_switch_to_navigation){
			if((navigation_status == status_recalculating || navigation_status == status_routing)){
				if(display_mode != NAVIGATION){
					display_mode_tmp = display_mode;
				}
				display_mode = NAVIGATION;
			}else if(display_mode == NAVIGATION){
				display_mode = display_mode_tmp;
			}
		}
		//*/
}

ISR(TIMER0_COMP_vect){//0.1ms timer
	t0cnt++;
	if(t0cnt == 10){//1ms
		t0cnt = 0;
		if(K58B_PIN & (1<<K58B)){
			k58b_timer=15;
		}else{
			if(k58b_timer > 0){
				k58b_timer--;
			}
		}
		if(door_delay){
			door_delay--;
		}
		if(draw_engine_cut_state){
			draw_engine_cut_state--;
		}
		if(!display_enable){
			display_enable_cnt++;
			if(display_enable_cnt > 99){
				display_enable = 1;
				display_enable_cnt = 0;
			}
		}
		set_backlight(k58b_pw);
		line_ms_timer++;

		if(line_ms_timer > 400){
			
				
			line_ms_timer = 0;
			line_shift_timer += 5;
			if(line_shift_timer > 0xFFF) line_shift_timer = 0;
		}
		if(send_can_lock < 20)
			if(!(line_ms_timer % 100))
				send_can_message = 1;
	
		if(can_mode == NO_CAN){
			// TODO: Calculate RPM
			if(rpm_cnt < 3001)
				rpm = (uint16_t) ((uint32_t) (600000 / tmp_rpm));
			else
				rpm = 0;
			rpm_cnt = 0;
			// TODO: Set can send timing dependent (10ms / 100ms / ...)
		}
	}
}

ISR(TIMER3_COMPA_vect){
	if(status == IGNITION_ON){
		driving_time[CUR]++;
		driving_time[AVG]++;
		driving_time_start++;
		uint32_t delta = 0;
		if(can_mode == CAN){
			new_val = ((id480_data[3] & 0x7F) << 8) + id480_data[2];
			if(new_val < old_val){
				delta = new_val - old_val + 0x7FFF;
			}else{
				delta = new_val - old_val;
			}
			old_val = new_val;
		}else{
			delta = (cons_cnt * 125) >> 1;
			speed_sum = hg_cnt;
		}
		calculate_consumption((uint16_t) delta, 1000);
		calculate_averages();
		old_val = new_val;
		start_cnt = 0;
		/*
		if(K58B_PORT & (1<<K58B)){
			K58B_PORT &= ~(1<<K58B);
		}else{
			K58B_PORT |= (1<<K58B);
		}
		*/
	}else{
		if(k15_delay_cnt){
			k15_delay_cnt--;
		}
		if(status == DOOR_OPEN && door_open_count < 305){
			door_open_count++;
		}else{
			door_open_count=0;
		}
	}
}

ISR(TIMER2_COMP_vect){
	start_cnt++;
	if(start_cnt > 57600){ // 4h ohne k15
		reset_averages_start();
		start_cnt = 0;
	}
}

ISR(INT6_vect){ //SDA -> RPM
	rpm_cnt++;
}

ISR(INT5_vect){ //SCL -> CONS
	cons_cnt++;
	cons_delta_ul += 62;
}

ISR(INT7_vect){ //EN_ADC1 -> HG
	hg_cnt++;
}
/*
ISR(INT0_vect){
	rpm++;
}

ISR(INT1_vect){
	rpm++;
}

ISR(INT2_vect){
	rpm++;
}

ISR(INT3_vect){
	rpm++;
}
ISR(TIMER2_OVF_vect){
	rpm++;
}

ISR(TIMER1_CAPT_vect){
	rpm++;
}
ISR(TIMER1_COMPB_vect){
	rpm++;
}
ISR(TIMER1_COMPC_vect){
	rpm++;
}
ISR(TIMER1_OVF_vect){
	rpm++;
}

ISR(TIMER0_OVF_vect){
	rpm++;
}

ISR(SPI_STC_vect){
	rpm++;
}

ISR(USART0_TX_vect){
	rpm++;
}


ISR(USART1_TX_vect){
	rpm++;
}

ISR(ANALOG_COMP_vect){
	rpm++;
}

ISR(ADC_vect){
	rpm++;
}

ISR(EE_READY_vect){
	rpm++;
}

ISR(TIMER3_CAPT_vect){
	rpm++;
}
ISR(TIMER3_COMPB_vect){
	rpm++;
}
ISR(TIMER3_COMPC_vect){
	rpm++;
}
ISR(TIMER3_OVF_vect){
	rpm++;
}

ISR(SPM_READY_vect){
	rpm++;
}
//*/