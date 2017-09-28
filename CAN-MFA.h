#ifndef CAN_SNIFFER_H
#define CAN_SNIFFER_H

#include "calculation.h"
#include <avr/eeprom.h>

#define MFA_BUTTONS_ACTIVE_LOW 1
#define VERSION 2
#if VERSION == 1
#undef HAVE_MCP_ADC
#undef HAVE_PWM_BACKLIGHT
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
#define LED_PORT PORTE
#define LED_DDR DDRE
#define LED PE2
#define CAN_PORT PORTD
#define CAN_DDR DDRD
#define TX_CAN PD5
#define RX_CAN PD6
#define CAN_RS PD7

#define MFA_SWITCH_PIN PINA
#define MFA_SWITCH_PORT PORTA
#define MFA_SWITCH_DDR DDRA
#define MFA_SWITCH_MODE PINA7
#define MFA_SWITCH_RES PINA6
#define MFA_SWITCH_MFA PINA5
//#define MFA_SWITCH_GND PINA7

#define PCA_PORT PORTG
#define PCA_DDR DDRG
#define DISABLE_PCA PG0

#elif VERSION == 2 
#define HAVE_MCP_ADC 1
#define HAVE_PWM_BACKLIGHT 1
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
#define K58B_PORT PORTE
#define K58B_PIN PINE
#define K58B PE6
#define LED_PORT PORTE
#define LED_DDR DDRE
#define LED PE3
#define CAN_PORT PORTD
#define CAN_DDR DDRD
#define TX_CAN PD5
#define RX_CAN PD6
#define CAN_RS PD7

#define EN3V3 PC5
#define CS_DOG PD4
#define CS_ADC PF0
#define EN_ADC0 PE4
#define EN_ADC1 PE5
#define EN_ADC6 PA3
#define EN_ADC7 PA2	 

#define MFA_SWITCH_PIN PINA
#define MFA_SWITCH_PORT PORTA
#define MFA_SWITCH_DDR DDRA
#define MFA_SWITCH_MODE PINA6///*PINA4 //*/PINA7
#define MFA_SWITCH_RES /*PINA7 //*/PINA4
#define MFA_SWITCH_MFA /*PINA6 //*/PINA5
#define MFA_SWITCH_GND PINA7///*PINA5 //*/PINA6


#define PCA_PORT PORTD
#define PCA_DDR DDRD
#define DISABLE_PCA PD7

#else
#error "Version definition error!"
#endif

//Display mode
#define NAVIGATION		0
#define SMALL_TEXT		1
#define MED_TEXT_TOP	2
#define MED_TEXT_BOT	3
#define SETTINGS		6
//#define BIG_TEXT	3
#define CAN_DATA	4//5
// MFA MODE
#define STARTUP 2
#define MODE1	0
#define MODE2	1
#define AVG	0
#define CUR	1
  
#define AMBIENT_FROST_TEMP 4
//CAN 
#define ID280 0
#define ID288 1
#define ID380 2
#define ID480 3
#define ID320 4
#define ID420 5
#define ID520 6
#define ID666 7
#define ID667 8

#define SPANNUNG1 2
#define SPANNUNG2 3
#define OELTEMP 0
#define AUSSENTEMP 1
#define SPANNUNG3 4
#define SPANNUNG4 5
#define GETRIEBETEMP 7
#define INNENTEMP 6


// display value med
#define VAL_CUR_SPEED 0
#define VAL_AVG_SPEED 1
#define VAL_CUR_CONS 2
#define VAL_AVG_CONS 3
#define VAL_VOLTA 4 //2
#define VAL_VOLTB 5
#define VAL_VOLTC 6 //2
#define VAL_VOLTD 7
#define VAL_ENGT 8 //4
#define	VAL_OIL 9
#define VAL_AMBIENT 10 //6
#define VAL_INT 11
#define VAL_GEARBXT 12
#define VAL_TIME 13
#define VAL_RPM 14
#define VAL_MANIFOLD 15
#define VAL_FUEL 16
#define VAL_RANGE 17 

// display value small
#define CAN_VALUES 2
#define ADC_VALUES 1
#define STANDARD_VALUES 0
#define CAN_VALUES2 3
#define MIN_MAX_VALUES 4
#define TEMPERATURE_VALUES 5
#define STARTSTOP 6

#define LINE_SHIFT_START 128

#define AVG_DIGIT 0x9D

/* Engine Cut Status*/
#define STARTING 1
#define RUNNING 2
#define CUTTING 3
#define CUT 4
#define DISABLED 5

#define BRAKE 0x03 // status2
#define THROTTLE 0x01 // status1
#define CLUTCH 0x08 // status1
#define AC 0x08 // status2
#define STARTED 0x10 // status2

typedef struct{
	uint8_t mfa;
	uint8_t res;
	uint8_t mode;
}mfa_t;

typedef enum stat{
	OFF = 0,
	IGNITION_ON,
	DOOR_OPEN,
}status_t;

extern volatile status_t status;
extern volatile status_t old_status;
extern volatile uint8_t off;

extern volatile uint8_t id280_data[8];
extern volatile uint8_t id288_data[8];
extern volatile uint8_t id380_data[8];
extern volatile uint8_t id480_data[8];
extern volatile uint8_t id320_data[8];
extern volatile uint8_t id420_data[8];
extern volatile uint8_t id520_data[8];
extern volatile uint8_t id666_data[8];
extern volatile uint8_t id667_data[8];

extern volatile uint8_t id280_valid;
extern volatile uint8_t id288_valid;
extern volatile uint8_t id380_valid;
extern volatile uint8_t id480_valid;
extern volatile uint8_t id320_valid;
extern volatile uint8_t id420_valid;
extern volatile uint8_t id520_valid;
extern volatile uint8_t id666_valid;
extern volatile uint8_t id667_valid;

// values from can data
extern volatile int16_t speed[2]; //0-317km/h
extern int16_t max_speed;
extern volatile int16_t speed_start; //0-317km/h
//extern volatile uint16_t speed[CUR]; //0-317km/h
//extern volatile uint16_t speed[AVG];
extern volatile uint16_t rpm;	//0-16000rpm
extern uint16_t max_rpm;
extern volatile int16_t engine_temperature;//-100-154 centigrade
extern int16_t max_engine_temp;
extern int16_t min_engine_temp;
extern volatile uint8_t fuel;	//0-100% or 0-80l
//extern volatile uint32_t cons_time[16];
//extern volatile uint8_t cons_cnt;
extern volatile uint16_t cons_delta_ul;
extern volatile uint16_t cons_delta_timer; 
//extern volatile uint32_t cons_ul[16];
extern volatile float cons_l_h[2];
extern volatile float cons_l_h_start;
extern volatile float cons_l_100km[2];
extern volatile float cons_l_100km_start;
extern volatile int16_t gra_speed; //id380D4
extern volatile uint8_t pedal_position; //id280D6
extern volatile uint8_t eng_status0; //id280D1
extern volatile uint8_t eng_status1; //id288D2
extern volatile uint8_t do_not_switch_to_navigation;

extern uint64_t can_speed_sum;
extern uint32_t can_speed_cnt;

extern volatile uint64_t speed_sum;
extern volatile uint64_t speed_sum_start;
extern volatile uint64_t cons_sum;
extern volatile uint64_t cons_sum_start;
extern volatile uint64_t avg_cnt;
extern volatile uint64_t avg_cnt_start;
extern volatile uint32_t driving_time[2];
extern volatile uint32_t driving_time_start;
extern volatile uint32_t distance[2];
extern volatile uint32_t distance_start;
extern volatile uint16_t range[2];
extern volatile uint32_t start_cnt;

extern volatile voltage_value_t starterbat;
extern volatile voltage_value_t zweitbat;
extern volatile voltage_value_t v_solar_plus;
extern volatile voltage_value_t v_solar_minus;
extern volatile int16_t in_temperature;
extern int16_t max_in_temp;
extern int16_t min_in_temp;
extern volatile int16_t gearbox_temperature;
extern int16_t max_gearbox_temp;
extern int16_t min_gearbox_temp;
extern volatile int16_t ambient_temperature;
extern int16_t max_ambient_temp;
extern int16_t min_ambient_temp;
extern volatile int16_t oil_temperature;
extern int16_t max_oil_temp;
extern int16_t min_oil_temp;
extern uint8_t EEMEM cal_water_temperature; //85; 
extern uint8_t EEMEM cal_voltage; // 171
extern uint8_t EEMEM cal_speed; // 169
extern uint8_t EEMEM cal_oil_temperature;
extern uint8_t EEMEM cal_in_temperature;
extern uint8_t EEMEM cal_consumption;
extern uint8_t EEMEM cal_gearbox_temperature;
extern uint8_t EEMEM cal_ambient_temperature;
extern uint8_t EEMEM cal_k15_delay;
extern uint8_t EEMEM cal_k58b_off_val;
extern uint8_t EEMEM cal_k58b_on_val;
extern volatile uint16_t avg_timer;
extern volatile uint16_t k58b_timer;
extern volatile uint32_t cons_timer;
extern volatile uint8_t can_status;
extern volatile uint8_t engine_cut;

extern volatile uint8_t display_mode;
extern volatile uint8_t display_mode_tmp;
extern volatile uint8_t old_display_mode;
extern volatile uint8_t display_enable;
extern volatile uint8_t display_value[7];
extern volatile uint8_t display_value_top;
extern volatile uint8_t display_value_bot;
extern volatile uint8_t old_display_value[7];

extern volatile uint8_t navigation_next_turn;
extern volatile uint8_t navigation_status;
extern volatile uint8_t navigation_status_old;
extern volatile uint32_t distance_to_next_turn;

extern volatile uint8_t radio_text[AUDIO_STR_LENGTH];

extern volatile uint8_t navi_old;
extern volatile uint16_t timer2_cnt;
extern volatile mfa_t mfa;
extern volatile mfa_t mfa_old;

extern volatile uint16_t line_ms_timer;
extern volatile uint16_t line_shift_timer;
extern volatile uint16_t line_wait_timer;
extern volatile uint16_t line3timer;
/*extern volatile uint16_t line4timer;
extern volatile uint16_t line5timer;
extern volatile uint16_t line6timer;
extern volatile uint16_t line7timer;
*/
extern volatile uint8_t send_can_message;
extern volatile uint8_t send_can_lock;

extern volatile uint8_t reversed;
extern volatile uint8_t underlined;

void init_spi_lcd(void);
void avr_init(void);
void display_task(void);
void app_task(void);
void twi_task(void);
int main(void);
#endif