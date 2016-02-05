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
#include "CAN-MFA.h"



extern volatile uint8_t i2crxdata[i2c_buffer_size]; 
extern volatile uint8_t i2ctxdata[i2c_buffer_size]; 
extern volatile uint8_t buffer_adr;
extern volatile uint8_t i2crxready;




uint8_t calculateID(char* name){
	//calculate an ID from the first 3 Letter of its name
	uint8_t ID;
	ID = (name[0]-48) * 3 + (name[1]-48) * 2 + (name[2]-48);
	ID = (ID << 1);
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
	while(len-->0){
		crc = crc8(crc, *data++);
	}
	return crc;
}

uint8_t serialize_rxdata(uint8_t size, volatile uint8_t buffer[size]){
	if(size != sizeof(rx_t)){
		return 0;
	}
	uint8_t i;
	for(i=0;i<32;i++){
		buffer[i] = rx.radio_text[i];
	}
	buffer[32] = rx.navigation_next_turn;
	buffer[33] = (uint8_t) ((rx.distance_to_next_turn & 0xFF00000) >> 24);
	buffer[34] = (uint8_t) ((rx.distance_to_next_turn & 0x00FF0000) >> 16);
	buffer[35] = (uint8_t) ((rx.distance_to_next_turn & 0x0000FF00) >> 8);
	buffer[36] = (uint8_t) (rx.distance_to_next_turn & 0x000000FF);
	buffer[37] = rx.cal_water_temperature;
	buffer[38] = rx.cal_voltage;
	buffer[39] = rx.cal_oil_temperature;
	buffer[40] = rx.cal_consumption;
	return 1;
}

uint8_t deserialize_rxdata(uint8_t size, volatile uint8_t buffer[size]){
	if(size != sizeof(rx)){
		return 0;
	}
	uint8_t i;
	for(i=0;i<32;i++){
		rx.radio_text[i] = buffer[i];
	}
	rx.navigation_next_turn = buffer[32];
	rx.distance_to_next_turn = ((uint32_t) (buffer[33]) << 24) + ((uint32_t) (buffer[34]) << 16) + ((uint16_t) (buffer[35]) << 8) + buffer[36];
	rx.cal_water_temperature = buffer[37];
	rx.cal_voltage = buffer[38];
	rx.cal_oil_temperature = buffer[39];
	rx.cal_consumption = buffer[40];
	return 1;
}

uint8_t serialize_txdata(tx_t tx, uint8_t size, volatile uint8_t buffer[size]){
	if(size != sizeof(tx_t)){
		return 0;
	}
	uint8_t i;
	for(i=0;i<32;i++){
		buffer[i] = tx.radio_text[i];
	}
	buffer[32] = tx.navigation_next_turn;
	buffer[33] = (uint8_t) ((tx.distance_to_next_turn & 0xFF00000) >> 24);
	buffer[34] = (uint8_t) ((tx.distance_to_next_turn & 0x00FF0000) >> 16);
	buffer[35] = (uint8_t) ((tx.distance_to_next_turn & 0x0000FF00) >> 8);
	buffer[36] = (uint8_t) (tx.distance_to_next_turn & 0x000000FF);
	buffer[37] = tx.cal_water_temperature;
	buffer[38] = tx.cal_voltage;
	buffer[39] = tx.cal_oil_temperature;
	buffer[40] = tx.cal_consumption;
	buffer[41] = (uint8_t) ((tx.voltage & 0xFF00) >> 8);
	buffer[42] = (uint8_t) (tx.voltage & 0x00FF);
	buffer[43] = tx.engine_temperature;
	buffer[44] = tx.ambient_temperature;
	buffer[45] = tx.oil_temperature;
	buffer[46] = (uint8_t) ((tx.consumption & 0xFF00) >> 8);
	buffer[47] = (uint8_t) (tx.consumption & 0x00FF);
	buffer[48] = (uint8_t) ((tx.average_consumption & 0xFF00) >> 8);
	buffer[49] = (uint8_t) (tx.average_consumption & 0x00FF);
	buffer[50] = (uint8_t) ((tx.range & 0xFF00) >> 8);
	buffer[51] = (uint8_t) (tx.range & 0x00FF);
	buffer[52] = (uint8_t) ((tx.speed & 0xFF00) >> 8);
	buffer[53] = (uint8_t) (tx.speed & 0x00FF);
	buffer[54] = (uint8_t) ((tx.average_speed & 0xFF00) >> 8);
	buffer[55] = (uint8_t) (tx.average_speed & 0x00FF);
	buffer[56] = (uint8_t) ((tx.rpm & 0xFF00) >> 8);
	buffer[57] = (uint8_t) (tx.rpm & 0x00FF);
	return 1;
}

uint8_t twi_rx_task(void){
	uint8_t i;
	uint8_t rx_size = sizeof(rx_t);
	// check rx crc
	uint8_t rx_crc = calculateCRC8(CRC_POLYNOME, i2crxdata, rx_size);
	if(rx_crc == i2crxdata[rx_size]){
		//crc is correct
		uint8_t ser_rx[rx_size];
		if(serialize_rxdata(rx_size, ser_rx)){
			// check if new data differs from current data object
			uint8_t ok = 0;
			for(i=0; i<rx_size; i++){
				if(i2crxdata[i] != ser_rx[i]) ok = 1;
			}
			if(ok){
				//we got new data -> replace the old object 
				if(deserialize_rxdata(sizeof(rx), i2crxdata)){
					// everything went fine -> clean the buffer
					for(i=0; i< i2c_buffer_size; i++){
						i2crxdata[i] = 0;
					}
				}else return 1; 
			}return 0;
		}else return 1; 
	}else return 1;
	return 0;
}



uint8_t twi_tx_task(void){	
	uint8_t tx_size = sizeof(tx_t);	
	// serialize tx object
	if(serialize_txdata(tx, tx_size, i2ctxdata)){
		//calculate CRC and append to i2ctxdata
		i2ctxdata[tx_size] = calculateCRC8(CRC_POLYNOME, i2ctxdata, tx_size);
		return 0;
	}	
	return 1;
}

void twi_task(void){
	// process data received
	twi_rx_task();
	uint8_t i;
	for(i=0;i<32;i++){
		radio_text[i] = rx.radio_text[i];
	}
	navigation_next_turn = rx.navigation_next_turn;
	distance_to_next_turn = rx.distance_to_next_turn;
	cal_water_temperature = rx.cal_water_temperature;
	cal_voltage = rx.cal_voltage;
	cal_oil_temperature = rx.cal_oil_temperature;
	cal_consumption = rx.cal_consumption;
	
	for(i=0;i<32;i++){
		tx.radio_text[i] = radio_text[i];
	}
	//prepare data for transmission
	tx.navigation_next_turn = navigation_next_turn;
	tx.distance_to_next_turn = distance_to_next_turn;
	
	tx.cal_water_temperature = cal_water_temperature;
	tx.cal_voltage = cal_voltage;
	tx.cal_oil_temperature = cal_oil_temperature;
	tx.cal_consumption = cal_consumption;

	tx.voltage = starterbat.integer * 100 + starterbat.fraction;
	tx.engine_temperature = engine_temperature;
	tx.ambient_temperature = ambient_temperature;
	tx.oil_temperature = oil_temperature;
	tx.consumption = cons_l_100km[0];
	tx.average_consumption = cons_l_100km[1];
	tx.range = range[0];
	tx.speed = speed[0];
	tx.average_speed = speed[2];
	tx.rpm = rpm;
	twi_tx_task();
}