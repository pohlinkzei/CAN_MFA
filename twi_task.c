/*
 * twi_task.c
 *
 * Created: 21.08.2014 09:32:08
 *  Author: Hubert
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "twi_slave.h"
#include "dog_display.h"
#include "twi_task.h"



extern volatile uint8_t i2crxdata[i2c_buffer_size]; 
extern volatile uint8_t i2ctxdata[i2c_buffer_size]; 
extern volatile uint8_t buffer_adr;
extern volatile uint8_t i2crxready;


uint8_t calculateID(char* name){
	//calculate an ID from the first 3 Letter of its name
	uint8_t ID;
	ID = (name[0]-48) * 3 + (name[1]-48) * 2 + (name[2]-48);
	ID = (ID >> 2);
	return ID;
}


uint8_t crc8(uint8_t crc, uint8_t data){
	uint8_t i, _data;
	_data = crc ^ data;
	for(i=0; i<8; i++){
		if((_data & 0x80) != 0){
			_data <<= 1;
			_data ^= 0x07;
		}else{
			_data <<= 1;
		}
	}
	return _data;
}

uint8_t calculateCRC8(uint8_t crc, volatile uint8_t* data, uint8_t len){
	while(len>0){
		crc = crc8(crc, *data++);
	}
	return crc;
}

uint8_t serialize_rxdata(rx_t *rx, uint8_t size, volatile uint8_t buffer[size]){
	if(sizeof(buffer) < size || size != sizeof(rx)){
		return 0;
	}
	buffer[0] = (uint8_t) ((rx->pwm_freq & 0xFF00) >> 8);
	buffer[1] = (uint8_t) (rx->pwm_freq & 0x00FF);
	buffer[2] = rx->cal_temperature;
	buffer[3] = rx->cal_voltage;
	buffer[4] = rx->water_value;
	buffer[5] = rx->time_value;
	return 1;
}

uint8_t deserialize_rxdata(rx_t *rx, uint8_t size, volatile uint8_t buffer[size]){
	if(sizeof(buffer) < size || size != sizeof(rx)){
		return 0;
	}
	rx->pwm_freq = ((uint16_t) (buffer[0]) << 8) + buffer[1];
	rx->cal_temperature = buffer[2];
	rx->cal_voltage = buffer[3];
	rx->water_value = buffer[4];
	rx->time_value = buffer[5];
	return 1;
}

uint8_t serialize_txdata(tx_t *tx, uint8_t size, volatile uint8_t buffer[size]){
	if(sizeof(buffer) < size || size != sizeof(tx)){
		return 0;
	}
	buffer[0] = (uint8_t) ((tx->pwm_freq & 0xFF00) >> 8);
	buffer[1] = (uint8_t) (tx->pwm_freq & 0x00FF);
	buffer[2] = tx->cal_temperature;
	buffer[3] = tx->cal_voltage;
	buffer[4] = tx->water_value;
	buffer[5] = tx->time_value;
	buffer[6] = (uint8_t) ((tx->vbat & 0xFF00) >> 8);
	buffer[7] = (uint8_t) (tx->vbat & 0x00FF);
	buffer[8] = tx->water_temp;
	buffer[9] = tx->fet_temp;
	return 1;
}
/*
uint8_t deserialize_txdata(tx_t tx, uint8_t size, volatile uint8_t buffer[size]){
	if(sizeof(buffer) < size || size != sizeof(tx)){
		return 0;
	}
	tx->pwm_freq = ((uint16_t) (buffer[0]) << 8) + buffer[1];
	tx->cal_temperature = buffer[2];
	tx->cal_voltage = buffer[3];
	tx->water_value = buffer[4];
	tx->time_value = buffer[5];
	tx->vbat = ((uint16_t) (buffer[6]) << 8) + buffer[7];
	tx->water_temp = buffer[8];
	tx->fet_temp = buffer[9];
	return 1;
}
//*/
uint8_t twi_rx_task(void){
	uint8_t i;
	uint8_t rx_size = sizeof(rx);
	// check rx crc
	uint8_t rx_crc = calculateCRC8(CRC_POLYNOME, i2crxdata, rx_size);
	if(rx_crc == i2crxdata[rx_size]){
		//crc is correct
		uint8_t ser_rx[rx_size];
		
		if(deserialize_rxdata(rx, sizeof(rx), ser_rx)){
			// check if new data differs from current data object
			uint8_t ok = 1;
			for(i=0; i<rx_size; i++){
				if(i2crxdata[i] != ser_rx[i]) ok = 0;
			}
			if(ok){
				//we got new data -> replace the old object 
				if(!serialize_rxdata(rx, rx_size, i2crxdata)){
					return 1;
				}else{
					// everything went fine -> clean the buffer
					for(i=0; i< i2c_buffer_size; i++){
						i2crxdata[i] = 0;
					}
				}
			}else return 1;
		}else return 1;
	}
	return 0;
	// end of rx data procession
}	
uint8_t twi_tx_task(void){
	uint8_t tx_size = sizeof(tx);		
	// serialize tx object
	if(serialize_txdata(tx, tx_size, i2ctxdata)){
		//calculate CRC and append to i2ctxdata
		i2ctxdata[tx_size] = calculateCRC8(CRC_POLYNOME, i2ctxdata, tx_size);
	}else return 1;
	return 0;
}
