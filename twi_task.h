#ifndef TWI_TASK_H
#define TWI_TASK_H

#include <avr/io.h>
#include "twi_slave.h"

#define CRC_POLYNOME 0xAB

typedef struct rxdata{
	uint8_t radio_text[32];
	uint8_t navigation_next_turn;
	long distance_to_next_turn;
	//navigation active?
	uint8_t cal_water_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	// other values from pwm module?
}rx_t;

typedef struct txdata{
	uint8_t radio_text[32];
	uint8_t navigation_next_turn;
	long distance_to_next_turn;
	//navigation active?
	uint8_t cal_water_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	// read only
	uint16_t voltage;
	int8_t engine_temperature;
	int8_t ambient_temperature;
	int8_t oil_temperature;
	int consumption;
	int average_consumption;
	int range;
	int speed;
	int average_speed;
	int rpm;
}tx_t;



rx_t rx;
tx_t tx;

uint8_t deb;
 
uint8_t twi_rx_task(void);
void twi_task(void);
uint8_t twi_tx_task(void);
uint8_t calculateCRC8(uint8_t crc, volatile uint8_t* data, uint8_t len);
uint8_t calculateID(char* name);

#endif