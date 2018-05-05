#ifndef TWI_TASK_H
#define TWI_TASK_H

#include <avr/io.h>
#include "twi_slave.h"
#include "calculation.h"
#include "CAN-MFA.h"

#define CRC_POLYNOME 0xAB

typedef struct rxdata {
	uint32_t distance_to_next_turn;
	char radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;
}rx_t;
	
typedef struct txdata {
	uint32_t distance_to_next_turn;
	uint8_t radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;	//navigation active?
}tx_t;
extern volatile rx_t rx;
extern volatile tx_t tx;

uint8_t deb;
 
void twi_task(void);
void twi_task2(void);
uint8_t calculateCRC8(uint8_t crc, volatile uint8_t* data, uint8_t len);
uint8_t calculateID(char* name);

#endif