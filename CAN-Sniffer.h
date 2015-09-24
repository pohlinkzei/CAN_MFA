#ifndef CAN_SNIFFER_H
#define CAN_SNIFFER_H

/*
#ifndef F_CPU
#define F_CPU 8000000
#endif
//*/
#include "calculation.h"
#include "can.h"
#define true 1
#define false 0

//Display
#define NAVIGATION	0
#define SMALL_TEXT	1
#define MED_TEXT	2
#define BIG_TEXT	255
#define TUER		128
#define CAN_DATA	3
// MFA MODE
#define STARTUP 2
#define MODE1	0
#define MODE2	1
#define AVG	0
#define CUR	1
  

//CAN 
#define ID280 0
#define ID288 1
#define ID380 2
#define ID480 3
#define ID320 4
#define ID420 5

// io
#define TKML PC7 
#define TKML_PORT PORTC
#define TKML_PIN PINC
#define REVERSE_PORT PORTC
#define REVERSE_PIN PINC
#define REVERSE_GEAR PC6
#define K15_PORT PORTE
#define K15_PIN PINE 
#define K15 PE7

#define SPANNUNG1 4
#define SPANNUNG2 3
#define OELTEMP 2
#define AUSSENTEMP 1

#define MFA_SWITCH_PIN PINA
#define MFA_SWITCH_PORT PORTA
#define MFA_SWITCH_DDR DDRA
#define MFA_SWITCH_MODE PINA5
#define MFA_SWITCH_RES PINA6
#define MFA_SWITCH_MFA PINA7
#define MFA_SWITCH_GEN PINA4

#define VAL_SPEED 0
#define VAL_CONS 1
#define VAL_VOLTA 2
#define VAL_VOLTB 3
#define VAL_WATER 4
#define	VAL_OIL 5
#define VAL_EXTTEMP 6
#define VAL_TIME 7
#define VAL_RPM 8
#define VAL_MANIFOLD 9
#define VAL_FUEL 10
#define VAL_RANGE 11 

#define CAN_VALUES 0
#define ADC_VALUES 1


#define AVG_DIGIT 0x9D

typedef struct{
	bool mfa;
	bool res;
	bool mode;
}mfa_t;


uint8_t id280_data[8];
uint8_t id288_data[8];
uint8_t id380_data[8];
uint8_t id480_data[8];
uint8_t id320_data[8];
uint8_t id420_data[8];

uint8_t id280_valid;
uint8_t id288_valid;
uint8_t id380_valid;
uint8_t id480_valid;
uint8_t id320_valid;
uint8_t id420_valid;

// values from can data
volatile uint16_t speed[2]; //0-317km/h
//volatile uint16_t speed[CUR]; //0-317km/h
//volatile uint16_t speed[AVG];
volatile uint16_t rpm;	//0-16000rpm
volatile int16_t eng_temp;//-100-154 centigrade
volatile uint8_t fuel;	//0-100% or 0-80l
volatile uint32_t cons_time[16];
volatile uint8_t cons_cnt;
volatile uint32_t cons_ul[16];
volatile float cons_l_h;
volatile float cons_l_100km[2];
//volatile float cons_l_100km[CUR];
//volatile float cons_l_100km[AVG];

volatile uint64_t speed_sum;
volatile uint64_t cons_sum;
volatile uint32_t avg_cnt;
volatile uint32_t driving_time[2];
volatile uint32_t distance[2];
volatile uint16_t range[2];
// volatile uint64_t driving_time[AVG];//odo
// volatile uint64_t driving_time[CUR];//trip
// volatile uint32_t distance[AVG];//odo
// volatile uint32_t distance[CUR];//trip
// volatile uint16_t range[CUR];
// volatile uint16_t range[AVG];

volatile voltage_value_t starterbat;
volatile voltage_value_t zweitbat;
volatile int16_t aussentemperatur;
volatile int16_t oeltemperatur;

volatile uint16_t avg_timer;
extern volatile uint16_t k58b_timer;
volatile uint32_t cons_timer;
volatile uint8_t cons_scale;
volatile uint8_t can_status;

volatile uint8_t display_values_small[4];
volatile uint8_t display_values_big[2];
volatile uint8_t mfa_mode;
volatile uint8_t display_mode;
volatile uint8_t old_mfa_mode;
volatile uint8_t old_display_mode;
volatile uint8_t display_enable;
volatile uint8_t display_value;
volatile uint8_t old_display_value;
volatile uint8_t max_display_value;

volatile int8_t next_turn;
volatile uint32_t distance_nt;

volatile uint8_t radio_text[64];



volatile uint16_t line_ms_timer;
volatile uint16_t line_shift_timer;
volatile uint16_t line_wait_timer;
volatile uint16_t line3timer;
/*volatile uint16_t line4timer;
volatile uint16_t line5timer;
volatile uint16_t line6timer;
volatile uint16_t line7timer;
*/
void init_spi_lcd(void);

void save_message(CAN_message msg);
void display_task(void);
void can_task(void);
void app_task(void);
void twi_task(void);
#endif