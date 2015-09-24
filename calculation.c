/*
 * calculation.c
 *
 * Created: 15.10.2014 15:05:39
 *  Author: Hubert
 */ 
#include <stdbool.h>
#include "calculation.h"

voltage_value_t calculate_voltage(uint16_t adc){
	uint32_t voltage32 = (719 * (uint32_t) adc);
	voltage32 = voltage32 / 256;
	voltage_value_t voltage;
	voltage.integer = voltage32 / 100;
	voltage.fraction = voltage32 - (100*voltage.integer);
	return voltage;
}	

void sprint_cur_speed(char* str, uint16_t integer){
	integer /= 10;
	if(integer > 255) integer = 255;
	uint8_to_string(str, (uint8_t) integer);
}	

void sprint_float(char* str, float _float){
	_float *= 10;
	uint16_t integer = (uint16_t) _float;
	sprint_avg_speed(str, integer);
}


void sprint_avg_speed(char* str, uint16_t integer){
	uint8_t one = 0;
	uint8_t ten = 0;
	uint8_t hundred = 0;
	uint8_t thousand = 0;
	uint8_t tenthousand = 0;
	one = integer % 10;
	ten = (integer % 100) / 10;
	hundred = (integer % 1000) / 100;
	thousand = (integer % 10000) / 1000;
	tenthousand = (integer % 100000) / 10000;
	if(thousand == 0){
		if(hundred == 0){
			if(ten == 0){
				str[0] = ' ';
				str[1] = ' ';
				str[2] = '0';
				str[3] = ',';
				str[4] = one + '0';
			}else{
				str[0] = ' ';
				str[1] = ' ';
				str[2] = ten + '0';
				str[3] = ',';
				str[4] = one + '0';
			}
		}else{
			str[0] = ' ';
			str[1] = hundred + '0';
			str[2] = ten + '0';
			str[3] = ',';
			str[4] = one + '0';
		}
	}else{
		str[0] = thousand + '0';
		str[1] = hundred + '0';
		str[2] = ten + '0';
		str[3] = ',';
		str[4] = one + '0';
	}
}

void sprint_voltage(char* str,voltage_value_t voltage){
	uint8_t tenner = 0;
	uint8_t singles = 0;
	singles = voltage.integer % 10;
	tenner = (voltage.integer - singles) / 10;
	str[0] = ' ';
	if(tenner == 0){
		str[1] = str[0];
		str[0] = ' ';
	}else{
		str[1] = tenner + '0';
	}
	str[2] = singles + '0';
	tenner = 0;
	singles = 0;
	str[3] = ',';
	singles = voltage.fraction % 10;
	tenner = (voltage.fraction - singles) / 10;
	str[4] = tenner + '0';
	str[5] = singles + '0';
}

void sprint_temperature(char* str,int16_t temperature){
	uint8_t one = 0, ten = 0, hundred = 0;
	bool _signed = false;
	if(temperature < 0){
		temperature *= -1;
		str[0] = '-';
		_signed = true;
		if(temperature > 99) temperature = 99;
	}
	one = temperature % 10;
	ten = (temperature % 100) / 10;
	hundred = (temperature % 1000) / 100;
	if(hundred == 0){
		if(ten == 0){
			if(_signed){
				str[1] = str[0];
			}else{
				str[1] = ' ';
			}
			str[0] = ' ';
		}else{
			if(_signed){
				str[0] = str[0];
			}else{
				str[0] = ' ';
			}
			str[1] = ten + '0';
		}
	}else{
		str[0] = hundred + '0';
		str[1] = ten + '0';
	}		
	str[2] = one + '0';
}

void uint8_to_string(char* str, uint8_t integer){
	uint8_t one = 0;
	uint8_t ten = 0;
	uint8_t hundred = 0;
	one = integer % 10;
	ten = (integer % 100) / 10;
	hundred = (integer % 1000) / 100;
	if(hundred == 0){
		if(ten == 0){
			str[0] = ' ';
			str[1] = ' ';
			str[2] = one + '0';
		}else{
			str[0] = ' ';
			str[1] = ten + '0';
			str[2] = one + '0';
		}
	}else{
		str[0] = hundred + '0';
		str[1] = ten + '0';
		str[2] = one + '0';
	}
}	
/***********************************************************************************************/
void uint16_to_string(char* str, uint16_t integer){
	uint8_t one = 0;
	uint8_t ten = 0;
	uint8_t hundred = 0;
	uint8_t thousand = 0;
	uint8_t tenthousand = 0;
	one = integer % 10;
	ten = (integer % 100) / 10;
	hundred = (integer % 1000) / 100;
	thousand = (integer % 10000) / 1000;
	tenthousand = (integer % 100000) / 10000;
	if(tenthousand == 0){
		if(thousand == 0){
			if(hundred == 0){
				if(ten == 0){
					str[0] = ' ';
					str[1] = ' ';
					str[2] = ' ';
					str[3] = ' ';
					str[4] = one + '0';
				}else{
					str[0] = ' ';
					str[1] = ' ';
					str[2] = ' ';
					str[3] = ten + '0';
					str[4] = one + '0';
				}
			}else{
				str[0] = ' ';
				str[1] = ' ';
				str[2] = hundred + '0';
				str[3] = ten + '0';
				str[4] = one + '0';
			}
		}else{
			str[0] = ' ';
			str[1] = thousand + '0';
			str[2] = hundred + '0';
			str[3] = ten + '0';
			str[4] = one + '0';
		}
	}else{
		str[0] = tenthousand + '0';
		str[1] = thousand + '0';
		str[2] = hundred + '0';
		str[3] = ten + '0';
		str[4] = one + '0';
	}
}	
/***********************************************************************************************/
void int16_to_string(char* str, int16_t integer){
	if(integer < 0){
		integer *= -1;
		str[0] = '-';
	}else{
		str[0] = ' ';
	}
	uint16_to_string(&str[1],integer);
}

int16_t calculate_temperature(uint16_t adc){//kty 81-110
	//t=0,64516129*adc-154,19354
	//#warning "TODO: update equation" done ;)
	int32_t adc_temp = 13213 * (int32_t) adc;
	adc_temp = adc_temp / 2048;
	adc_temp = adc_temp - 1547;
	adc_temp = adc_temp / 10;
	return (int16_t) adc_temp;
}


int16_t calculate_oil_temperature(uint16_t adc){//oil temp KTY19-6
	//t=0,59271677*adc-237,37865
	//#warning "TODO: update equation" done ;)
	int32_t adc_temp = 12139 * (int32_t) adc;
	adc_temp = adc_temp / 2048;
	adc_temp = adc_temp - 2374;
	adc_temp = adc_temp / 10;
	return (int16_t) adc_temp;
}

void stringcopy(char* source, char* destination, uint8_t length){
	uint8_t i;
	for(i=0;i<length;i++){
		destination[i] = source[i];
	}
}