#ifndef TWI_TASK_H
#define TWI_TASK_H

#include <avr/io.h>
#include "twi_slave.h"

#define CRC_POLYNOME 0xAB

typedef struct rxdata {
	char radio_text[32];
	uint8_t navigation_next_turn;
	uint32_t distance_to_next_turn;
	//navigation active?
	uint8_t cal_water_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	// other values from pwm module?
}rx_t;

typedef struct txdata {
	char radio_text[32];
	uint8_t navigation_next_turn;
	uint32_t distance_to_next_turn;
	//navigation active?
	uint8_t cal_water_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	// read only
	uint16_t voltage;
	uint8_t water_temperature;
	uint8_t ambient_temperature;
	uint8_t oil_temperature;
	uint16_t consumption;
	uint16_t average_consumption;
	uint16_t range;
	uint16_t speed;
	uint16_t average_speed;
	uint16_t rpm;
}tx_t;

rx_t *rx;// = (rx_t*) malloc(sizeof(rx_t));
tx_t *tx;// = (tx_t*) malloc(sizeof(tx_t));

 
uint8_t twi_rx_task(void);
uint8_t twi_tx_task(void);
uint8_t calculateCRC8(uint8_t crc, volatile uint8_t* data, uint8_t len);
uint8_t calculateID(char* name);

#endif