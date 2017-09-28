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
#include <avr/eeprom.h>
#include "CAN-MFA.h"


extern volatile rx_t rx;
extern volatile tx_t tx;

extern volatile uint8_t i2crxdata[i2c_buffer_size]; 
extern volatile uint8_t i2ctxdata[i2c_buffer_size]; 
extern volatile uint8_t buffer_adr;
extern volatile uint8_t i2crxready;

extern uint8_t cal_ambient_temperature EEMEM;
extern uint8_t cal_voltage EEMEM;
extern uint8_t cal_speed EEMEM;
extern uint8_t cal_oil_temperature EEMEM;
extern uint8_t cal_consumption EEMEM;

uint16_t i2ctimeout = 0; 

uint8_t calculateID(char* name){
	//calculate an ID from the first 3 Letter of its name
	uint8_t ID;
	ID = (name[0]-48) * 3 + (name[1]-48) * 2 + (name[2]-48);
	ID = (ID << 1);
	return ID;
}


uint8_t crc8(uint8_t crc, uint8_t data){
	uint8_t i, _data = 0;
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
	
	/*
	uint32_t distance_to_next_turn;
	char radio_text[AUDIO_STR_LENGTH];
	uint8_t navigation_next_turn;
	uint8_t cal_ambient_temperature;
	uint8_t cal_voltage;
	uint8_t cal_oil_temperature;
	uint8_t cal_consumption;
	uint8_t cal_speed;
	*/
	uint8_t i;
	buffer[0] = (uint8_t) ((rx.distance_to_next_turn & 0xFF00000) >> 24);
	buffer[1] = (uint8_t) ((rx.distance_to_next_turn & 0x00FF0000) >> 16);
	buffer[2] = (uint8_t) ((rx.distance_to_next_turn & 0x0000FF00) >> 8);
	buffer[3] = (uint8_t) (rx.distance_to_next_turn & 0x000000FF);
	for(i=0;i<AUDIO_STR_LENGTH;i++){
		buffer[i + 4] = rx.radio_text[i];
	}
	buffer[AUDIO_STR_LENGTH + 4] = rx.navigation_next_turn;
	buffer[AUDIO_STR_LENGTH + 5] = rx.cal_ambient_temperature;
	buffer[AUDIO_STR_LENGTH + 6] = rx.cal_voltage;
	buffer[AUDIO_STR_LENGTH + 7] = rx.cal_oil_temperature;
	buffer[AUDIO_STR_LENGTH + 8] = rx.cal_consumption;
	buffer[AUDIO_STR_LENGTH + 9] = rx.cal_speed;
	return 1;
}

uint8_t deserialize_rxdata(uint8_t size, volatile uint8_t buffer[size]){
	if(size != sizeof(rx)){
		return 0;
	}
	uint8_t i;

	rx.distance_to_next_turn =	  ((uint32_t) (buffer[0]) << 24) 
								+ ((uint32_t) (buffer[1]) << 16)
								+ ((uint16_t) (buffer[2]) << 8) 
								+ buffer[3];

	for(i=0;i<AUDIO_STR_LENGTH;i++){
		rx.radio_text[i] = buffer[i+4];
	}
	rx.navigation_next_turn =	buffer[AUDIO_STR_LENGTH + 4];	
	rx.cal_ambient_temperature =	buffer[AUDIO_STR_LENGTH + 5];
	rx.cal_voltage =			buffer[AUDIO_STR_LENGTH + 6];
	rx.cal_oil_temperature =	buffer[AUDIO_STR_LENGTH + 7];
	rx.cal_consumption =		buffer[AUDIO_STR_LENGTH + 8];
	rx.cal_speed =				buffer[AUDIO_STR_LENGTH + 9];
	return 1;
}

uint8_t serialize_txdata(tx_t tx, uint8_t size, volatile uint8_t buffer[size]){
	if(size != sizeof(tx_t)){
		return 0;
	}
	/*
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
	int8_t water_temperature;
	int8_t ambient_temperature;
	int8_t oil_temperature;
	*/
	uint8_t i;
	
	buffer[0] = (uint8_t) ((tx.distance_to_next_turn & 0xFF00000) >> 24);
	buffer[1] = (uint8_t) ((tx.distance_to_next_turn & 0x00FF0000) >> 16);
	buffer[2] = (uint8_t) ((tx.distance_to_next_turn & 0x0000FF00) >> 8);
	buffer[3] = (uint8_t) (tx.distance_to_next_turn & 0x000000FF);
	
	buffer[4] = (uint8_t) ((tx.voltage & 0xFF00) >> 8);
	buffer[5] = (uint8_t) (tx.voltage & 0x00FF);
	
	buffer[6] = (uint8_t) ((tx.consumption & 0xFF00) >> 8);
	buffer[7] = (uint8_t) (tx.consumption & 0x00FF);
	buffer[8] = (uint8_t) ((tx.average_consumption & 0xFF00) >> 8);
	buffer[9] = (uint8_t) (tx.average_consumption & 0x00FF);
	buffer[10] = (uint8_t) ((tx.range & 0xFF00) >> 8);
	buffer[11] = (uint8_t) (tx.range & 0x00FF);
	buffer[12] = (uint8_t) ((tx.speed & 0xFF00) >> 8);
	buffer[13] = (uint8_t) (tx.speed & 0x00FF);
	buffer[14] = (uint8_t) ((tx.average_speed & 0xFF00) >> 8);
	buffer[15] = (uint8_t) (tx.average_speed & 0x00FF);
	buffer[16] = (uint8_t) ((tx.rpm & 0xFF00) >> 8);
	buffer[17] = (uint8_t) (tx.rpm & 0x00FF);
	
	for(i=0;i<AUDIO_STR_LENGTH;i++){
		buffer[i + 18] = tx.radio_text[i];
	}
	buffer[AUDIO_STR_LENGTH + 18] = tx.navigation_next_turn;
	buffer[AUDIO_STR_LENGTH + 19] = tx.cal_ambient_temperature;
	buffer[AUDIO_STR_LENGTH + 20] = tx.cal_voltage;
	buffer[AUDIO_STR_LENGTH + 21] = tx.cal_oil_temperature;
	buffer[AUDIO_STR_LENGTH + 22] = tx.cal_consumption;
	buffer[AUDIO_STR_LENGTH + 23] = tx.cal_speed;
	buffer[AUDIO_STR_LENGTH + 24] = tx.engine_temperature;
	buffer[AUDIO_STR_LENGTH + 25] = tx.ambient_temperature;
	buffer[AUDIO_STR_LENGTH + 26] = tx.oil_temperature;
	
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

void clear_rx_data(rx_t rx){
	memset(&rx, 0x00, sizeof(rx_t));
}

void clear_tx_data(tx_t tx){
	memset(&tx, 0x00, sizeof(tx_t));
}

void twi_task(void){
	// process data received
	if(!i2crxready){
		i2ctimeout++;
		if(i2ctimeout > 150){
			clear_rx_data(rx);
			clear_tx_data(tx);
			return;
		}
	}else{
		i2ctimeout = 0;
	}

	if(twi_rx_task()){
		if(rx.radio_text[0] != 0)
			strncpy((char*) radio_text, (char*) rx.radio_text, AUDIO_STR_LENGTH);
		else{
									   //0123456789012345
			strncpy((char*) radio_text, "   CANmfa v1.1 ", 15);
		}
		uint8_t i;

		navigation_next_turn = rx.navigation_next_turn & 0x1F;
		navigation_status = (rx.navigation_next_turn >> 5) & 0x07;
	
		distance_to_next_turn = rx.distance_to_next_turn;
		/*
		uint8_t u8_tmp = eeprom_read_byte(&cal_ambient_temperature);
		if(u8_tmp != rx.cal_ambient_temperature && rx.cal_ambient_temperature != 0){
			eeprom_write_byte(&cal_ambient_temperature, rx.cal_ambient_temperature);
		}

		u8_tmp = eeprom_read_byte(&cal_oil_temperature);
		if(u8_tmp != rx.cal_oil_temperature && rx.cal_oil_temperature != 0){
			eeprom_write_byte(&cal_oil_temperature, rx.cal_oil_temperature);
		}
		u8_tmp = eeprom_read_byte(&cal_consumption);
		if(u8_tmp != rx.cal_consumption && rx.cal_consumption != 0){
			eeprom_write_byte(&cal_consumption, rx.cal_ambient_temperature);
		}
		u8_tmp = eeprom_read_byte(&cal_voltage);
		if(u8_tmp != rx.cal_voltage && rx.cal_voltage != 0){
			eeprom_write_byte(&cal_voltage, rx.cal_voltage);
		}

		u8_tmp = eeprom_read_byte(&cal_speed);
		if(u8_tmp != rx.cal_speed && rx.cal_speed != 0){
			eeprom_write_byte(&cal_speed, rx.cal_speed);
		}
		//*/
		for(i=0;i<AUDIO_STR_LENGTH;i++){
			tx.radio_text[i] = radio_text[i];
		}
	}
	//prepare data for transmission
	/*
	sprintf(val, "%i\n", eeprom_read_byte(&cal_ambient_temperature));
	eeprom_write_byte(&cal_ambient_temperature, uart_get_int());
	//*/
	//*
	tx.navigation_next_turn = navigation_next_turn + (navigation_status << 5);
	tx.distance_to_next_turn = distance_to_next_turn;
/*
	tx.cal_ambient_temperature = eeprom_read_byte(&cal_ambient_temperature);
	tx.cal_voltage = eeprom_read_byte(&cal_voltage);
	tx.cal_oil_temperature = eeprom_read_byte(&cal_oil_temperature);
	tx.cal_consumption = eeprom_read_byte(&cal_consumption);

	tx.voltage = starterbat.integer * 100 + starterbat.fraction;
	tx.engine_temperature = engine_temperature;
	tx.ambient_temperature = ambient_temperature;
	tx.oil_temperature = oil_temperature;
	tx.consumption = cons_l_100km[0];
	tx.average_consumption = cons_l_100km[1];
	tx.range = range[0];
	tx.speed = speed[0];
	tx.average_speed = speed[1];
	tx.rpm = rpm;
	//*/
	twi_tx_task();
}
