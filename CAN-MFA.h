#ifndef CAN_SNIFFER_H
#define CAN_SNIFFER_H

#include "calculation.h"
#include <avr/eeprom.h>
#define NO_CAN 1
#define CAN 0
#define NO_I2C 0
#define I2C 1
#define MFA_BUTTONS_ACTIVE_LOW 1

#define HAVE_PWM_BACKLIGHT 1
// io

// UART0: K-Line
// UART1: USB-Serial
// ACD: choose DogM132 or DogM128

// PORTA
#define MFA_SWITCH_PIN PINA
#define MFA_SWITCH_PORT PORTA
#define MFA_SWITCH_DDR DDRA
#define MFA_SWITCH_MODE PINA6///*PINA4 //*/PINA7
#define MFA_SWITCH_RES /*PINA7 //*/PINA4
#define MFA_SWITCH_MFA /*PINA6 //*/PINA5
#define MFA_SWITCH_GND PINA7///*PINA5 //*/PINA6

// PORTB
#define LED_PORT PORTB
#define LED_DDR DDRB
#define LED PB6 // OC1B
#define CS_DOG PB5

// PORTC
#define K15_PORT PORTC
#define K15_PIN PINC
#define K15 PC0
#define TKML PC2
#define TKML_PORT PORTC
#define TKML_PIN PINC
#define DPLUS_PORT PORTC
#define DPLUS_PIN PINC
#define DPLUS PC3
#define MKL PC6
#define MKL_PORT PORTC
#define MKL_PIN PINC

// PORTD
#define CAN_PORT PORTD
#define CAN_DDR DDRD
#define TX_CAN PD5
#define RX_CAN PD6
#define CAN_RS PD7
#define USB_PORT PORTD
#define USB_DDR DDRD
#define USB_ACTIVE PD4 // ICP4
#define PCA_PORT PORTD
#define PCA_DDR DDRD
#define DISABLE_PCA PD7

// PORTE
#define K58B_PORT PORTE
#define K58B_PIN PINE
#define K58B PE4
#define CONS_PORT PORTE
#define CONS_PIN PINE
#define CONS_IN PE6
#define RPM_PORT PORTE
#define RPM_PIN PINE
#define RPM_IN PE5
#define HG_PORT PORTE
#define HG_PIN PINE
#define HG PE5
#define KLINE_PORT PORTE
#define KLINE_DDR DDRE
#define KLINE_TX PE1
#define KLINE_RX PE0

enum displaymode {
	NAVIGATION=0,
	SMALL_TEXT,
	MED_TEXT_TOP,
	MED_TEXT_BOT,
	CAN_DATA,
	TOP_LINE,
	SETTINGS,
	INVALID,
};

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
#define ID480 2
#define ID380 3
#define ID320 4
#define ID420 5
#define ID520 6
#define ID666 7
#define ID667 8


/*
aux_temp
oiltemp
Gearbox_temp
Manifold
Starterbat
Zweitbat
Solar
D+/nc
*/

#define AUSSENTEMP 0
#define OELTEMP 1
#define GETRIEBETEMP 6
#define MANIFOLD 7
#define SPANNUNG1 2
#define SPANNUNG2 3
#define SPANNUNG3 4
#define SPANNUNG4 5

enum top_value {
	RADIO_TEXT,
	VOLTAGES0,
	VOLTAGES1,
	VOLTAGES2,
	VOLTAGES3,
	VOLTAGES4,
	VOLTAGES5,
	TEMPERATURES0,
	TEMPERATURES1,
	TEMPERATURES2,
};
// display value med
enum med_value {
	 VAL_CUR_SPEED,
	 VAL_AVG_SPEED,
	 VAL_CUR_CONS,
	 VAL_AVG_CONS,
	 VAL_CONS_START,
	 VAL_CONS_KM_L_CUR,
	 VAL_CONS_KM_L_AVG,
	 VAL_CONS_KM_L_START,
	 VAL_VOLTA,
	 VAL_VOLTB,
	 VAL_VOLTC,
	 VAL_VOLTD,
	 VAL_ENGT,
	 VAL_OIL,
	 VAL_AMBIENT,
	 VAL_GEARBXT,
	 VAL_TIME,
	 VAL_RPM,
	 VAL_MANIFOLD,
	 VAL_FUEL,
	 VAL_RANGE,
};

enum small_value {
	CAN_VALUES,
	ADC_VALUES,
	STANDARD_VALUES,
	CAN_VALUES2,
	MIN_MAX_VALUES,
	TEMPERATURE_VALUES,
	STARTSTOP,
	RANGE,
};
#define LINE_SHIFT_START 128

#define AVG_DIGIT 0x9D

/* Engine Cut Status*/
#define STARTING 1
#define RUNNING 2
#define CUTTING 3
#define CUT 4
#define DISABLED 5
#define WAITING 6
#define REMIND_WAITING 7

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

typedef enum eng_type{
	PETROL = 0,
	TDI_CAN,
	TDI_NOCAN,
}engine_type_t;

extern volatile status_t status;
extern status_t old_status;
//extern uint8_t off;

extern volatile uint8_t id280_data[8];
extern volatile uint8_t id288_data[8];
extern volatile uint8_t id380_data[8];
extern volatile uint8_t id480_data[8];
extern volatile uint8_t id320_data[8];
extern volatile uint8_t id420_data[8];
extern volatile uint8_t id520_data[8];
extern volatile uint8_t id666_data[8];
extern volatile uint8_t id667_data[8];

#define id280_valid 0
#define id288_valid 1
#define id380_valid 2
#define id480_valid 3
#define id320_valid 4
#define id420_valid 5
#define id520_valid 6
#define id666_valid 7
#define id667_valid 8

uint16_t can_id_valid;

// values from can data
extern int16_t speed[2]; //0-317km/h
extern int16_t max_speed;
extern int16_t speed_start; //0-317km/h
//extern volatile uint16_t speed[CUR]; //0-317km/h
//extern volatile uint16_t speed[AVG];
extern volatile uint16_t rpm;	//0-16000rpm
extern uint16_t max_rpm;
extern int16_t engine_temperature;//-100-154 centigrade
extern int16_t max_engine_temp;
extern int16_t min_engine_temp;
extern uint8_t fuel;	//0-100% or 0-80l
//extern volatile uint32_t cons_time[16];
//extern volatile uint8_t cons_cnt;
extern volatile uint16_t cons_delta_ul;
extern uint16_t cons_delta_timer; 
//extern volatile uint32_t cons_ul[16];
extern float cons_l_h[2];
extern float cons_l_h_start;
extern float cons_km_l[2];
extern float cons_km_l_start;
extern float cons_l_100km[2];
extern float cons_l_100km_start;
extern int16_t gra_speed; //id380D4
extern uint8_t pedal_position; //id280D6
extern uint8_t eng_status0; //id280D1
extern uint8_t eng_status1; //id288D2
extern uint8_t do_not_switch_to_navigation;

extern uint64_t can_speed_sum;
extern uint32_t can_speed_cnt;

extern uint8_t tkol;
extern uint8_t handbrake;
extern int8_t g266;
extern int8_t g2;
extern uint8_t sidelight;

extern volatile uint64_t speed_sum;
extern uint64_t speed_sum_start;
extern float cons_sum;
extern uint64_t cons_sum_start;
extern uint64_t avg_cnt;
extern uint64_t avg_cnt_start;
extern volatile uint32_t driving_time[2];
extern volatile uint32_t driving_time_start;
extern uint32_t distance[2];
extern uint32_t distance_start;
extern uint16_t range_start;
extern uint16_t range_avg;
extern volatile uint32_t start_cnt;

extern voltage_value_t starterbat;
extern voltage_value_t zweitbat;
extern voltage_value_t v_solar_plus;
extern voltage_value_t entlastungsbat;
extern int16_t gearbox_temperature;
extern int16_t max_gearbox_temp;
extern int16_t min_gearbox_temp;
extern int16_t ambient_temperature;
extern int16_t max_ambient_temp;
extern int16_t min_ambient_temp;
extern int16_t oil_temperature;
extern uint16_t manifold;
extern uint16_t min_manifold;
extern uint16_t max_manifold;
extern int16_t max_oil_temp;
extern int16_t min_oil_temp;
extern uint8_t EEMEM cal_engine_type;
extern uint8_t EEMEM cal_i2c_mode;
extern uint8_t EEMEM cal_water_temperature; //85;
extern uint8_t EEMEM cal_voltage1; // 171
extern uint8_t EEMEM cal_voltage2; // 171
extern uint8_t EEMEM cal_voltage3; // 171
extern uint8_t EEMEM cal_voltage4; // 171
extern uint8_t EEMEM cal_speed; // 169
extern uint8_t EEMEM cal_oil_temperature;
extern uint8_t EEMEM cal_manifold;
extern uint8_t EEMEM cal_consumption;
extern uint8_t EEMEM cal_gearbox_temperature;
extern uint8_t EEMEM cal_ambient_temperature;
extern uint8_t EEMEM cal_k15_delay;
extern uint8_t EEMEM cal_k58b_off_val;
extern uint8_t EEMEM cal_k58b_on_val;
extern uint8_t EEMEM cal_can_mode;
extern uint8_t EEMEM cal_startstop_enabled;
extern uint8_t mkl;
extern uint16_t avg_timer;
extern volatile uint16_t k58b_timer;
extern uint32_t cons_timer;
extern volatile uint8_t can_status;
extern uint8_t engine_cut;
extern uint8_t engine_cut_old;
extern uint16_t draw_engine_cut_state;
extern uint8_t startstop_enabled;
extern engine_type_t engine_type;

extern uint8_t display_mode;
extern uint8_t display_mode_tmp;
extern uint8_t old_display_mode;
extern uint8_t display_value[8];
extern uint8_t display_value_top;
extern uint8_t display_value_bot;
extern uint8_t old_display_value[8];

extern uint8_t navigation_next_turn;
extern uint8_t navigation_status;
extern uint8_t navigation_status_old;
extern uint32_t distance_to_next_turn;

extern uint8_t radio_text[AUDIO_STR_LENGTH];
extern volatile uint8_t can_mode;

extern uint8_t navi_old;
extern volatile uint16_t timer2_cnt;
extern mfa_t mfa;
extern mfa_t mfa_old;

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

extern uint8_t reversed;
extern uint8_t underlined;

void init_spi_lcd(void);
void avr_init(void);
void display_task(void);
void app_task(void);
void twi_task(void);
int main(void);
void enable_mfa_switch(void);
void disable_mfa_switch(void);
uint8_t read_mfa_switch(uint8_t button);

#endif