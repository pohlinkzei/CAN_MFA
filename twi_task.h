#ifndef TWI_TASK_H
#define TWI_TASK_H

#include <avr/io.h>
#include "twi_slave.h"
#include "calculation.h"
#include "CAN-MFA.h"

#define CRC_POLYNOME 0xAB

/*
typedef struct rxdata{
	uint8_t radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;
	uint32_t distance_to_next_turn;
	//navigation active?
	uint8_t cal_water_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	uint8_t cal_speed;
	//uint8_t dummy;
	// other values from pwm module?
}rx_t;

typedef struct txdata{
	uint8_t radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;
	uint32_t distance_to_next_turn;
	//navigation active?
	uint8_t cal_water_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	uint8_t cal_speed;
	// read only
	uint16_t voltage;
	int8_t engine_temperature;
	int8_t ambient_temperature;
	int8_t oil_temperature;
	uint16_t consumption;
	uint16_t average_consumption;
	uint16_t range;
	uint16_t speed;
	uint16_t average_speed;
	uint16_t rpm;
}tx_t;
*/

typedef struct rxdata {
	uint32_t distance_to_next_turn;
	char radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;
	uint8_t cal_ambient_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;	
	uint8_t cal_speed;
}rx_t;
	
typedef struct txdata {
	uint32_t distance_to_next_turn;
	uint16_t voltage;
	uint16_t consumption;
	uint16_t average_consumption;
	uint16_t range;
	uint16_t speed;
	uint16_t average_speed;
	int16_t rpm;
	uint8_t radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;	//navigation active?
	uint8_t cal_ambient_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;	
	uint8_t cal_speed;
	// read only
	int8_t engine_temperature;
	//int8_t water_temperature;
	int8_t ambient_temperature;
	int8_t oil_temperature;
	uint8_t dummy;
}tx_t;
extern volatile rx_t rx;
extern volatile tx_t tx;

uint8_t deb;
 
void twi_task(void);
void twi_task2(void);
uint8_t calculateCRC8(uint8_t crc, volatile uint8_t* data, uint8_t len);
uint8_t calculateID(char* name);

#endif