/*
 * calculation.c
 *
 * Created: 15.10.2014 15:05:39
 *  Author: Hubert
 */ 
//#include <stdbool.h>
#include "CAN-MFA.h"
#include "calculation.h"

voltage_value_t calculate_voltage(uint16_t adc){
	uint32_t voltage32 = (719 * (uint32_t) adc);
	voltage32 = voltage32 * eeprom_read_byte(&cal_voltage) / 32768;
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

uint32_t round_distance(uint32_t distance){
	if (distance >= 10000)
	return ((distance + 500) / 1000) * 1000;
	else if (distance >= 1000)
	return ((distance+50) / 100) * 100;
	else if (distance >= 300)
	return ((distance+ 13) / 25) * 25;
	else if (distance >= 50)
	return ((distance + 5) / 10) * 10;
	else
	return distance;
}

void sprint_distance(char* str, uint64_t distance){
	if (distance >= 10000){
		uint16_to_string(str, distance / 1000);
		str[5] = 'k';
		str[6] = 'm';
	}else if (distance >= 1000){
		sprint_float(str, ((float) distance / 1000));
		str[5] = 'k';
		str[6] = 'm';
	}else if (distance >= 300){
		//sprintf(str, "%.0f m ", (((double) distance) / 25) * 25);
		uint16_to_string(str, (distance / 25) * 25);
		str[5] = 'm';
		str[6] = ' ';
	}else if (distance >= 50){
		//sprintf(str, "%.0f m ", (((double) distance) / 10) * 10);
		uint16_to_string(str, (distance / 10) * 10);
		str[5] = 'm';
		str[6] = ' ';
	}else if (distance >= 10){
		//sprintf(str, "%.0f m ", ((double) distance));
		uint16_to_string(str, distance);
		str[5] = 'm';
		str[6] = ' ';
	}else{
		//sprintf(str, "%.1f m ", ((double) distance));
		uint16_to_string(str, distance);
		str[5] = 'm';
		str[6] = ' ';
	}
}


void sprint_avg_speed(char* str, uint16_t integer){
	uint8_t one = 0;
	uint8_t ten = 0;
	uint8_t hundred = 0;
	uint8_t thousand = 0;
	//uint8_t tenthousand = 0;
	one = integer % 10;
	ten = (integer % 100) / 10;
	hundred = (integer % 1000) / 100;
	thousand = (integer % 10000) / 1000;
	//tenthousand = (integer % 100000) / 10000;
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


void sprint_voltage_precision(char* str,voltage_value_t voltage, uint8_t prec){
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
	if(prec!=0){ // 12,x
		tenner = 0;
		singles = 0;
		str[3] = ',';
		if(prec==2){ // 12,34
			singles = voltage.fraction % 10;
			str[5] = singles + '0';
		}else{
			singles = -5; // round
		}
		tenner = (voltage.fraction - singles) / 10;
		str[4] = tenner + '0';
	}
}

void sprint_voltage(char* str,voltage_value_t voltage){
	sprint_voltage_precision(str,voltage, 2);
	#if 0
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
	#endif
}

void sprint_temperature(char* str,int16_t temperature){
	uint8_t one = 0, ten = 0, hundred = 0;
	uint8_t _signed = 0;
	if(temperature < 0){
		temperature *= -1;
		str[0] = '-';
		_signed = 1;
	}
	if(temperature > 149 || (temperature > 49 && _signed)){
		str[0]=' ';
		str[1]='-';
		str[2]='-';	
		return;
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

/*
uint32_t round_distance(uint32_t distance){
	if (distance >= 10000)
	return ((distance + 500) / 1000) * 1000;
	else if (distance >= 1000)
	return ((distance+50) / 100) * 100;
	else if (distance >= 300)
	return ((distance+ 13) / 25) * 25;
	else if (distance >= 50)
	return ((distance + 5) / 10) * 10;
	else
	return distance;
}


void sprint_distance(char* str, uint64_t distance){
	if (distance >= 10000){
		uint16_to_string(str, distance / 1000);
		str[5] = 'k';
		str[6] = 'm';
	}else if (distance >= 1000){
		sprint_float(str, ((float) distance / 1000));
		str[5] = 'k';
		str[6] = 'm';
	}else if (distance >= 300){
		//sprintf(str, "%.0f m ", (((double) distance) / 25) * 25);
		uint16_to_string(str, (distance / 25) * 25);
		str[5] = 'm';
		str[6] = ' ';
	}else if (distance >= 50){
		//sprintf(str, "%.0f m ", (((double) distance) / 10) * 10);
		uint16_to_string(str, (distance / 10) * 10);
		str[5] = 'm';
		str[6] = ' ';
	}else if (distance >= 10){
		//sprintf(str, "%.0f m ", ((double) distance));
		uint16_to_string(str, distance);
		str[5] = 'm';
		str[6] = ' ';
	}else{
		//sprintf(str, "%.1f m ", ((double) distance));
		uint16_to_string(str, distance);
		str[5] = 'm';
		str[6] = ' ';
	}
}
*/

int16_t calculate_ambient_temperature(uint16_t adc){//kty 81-110
	//t=((840*adc)/128-1557)/10
	volatile int32_t adc_temp = 840 * adc;
	adc_temp = adc_temp / 128;
	volatile uint16_t cal_offset = 1429;
	cal_offset += eeprom_read_byte(&cal_ambient_temperature); // 1429 - 1684
	adc_temp = adc_temp - cal_offset;
	adc_temp = adc_temp / 10;
	return (int16_t) adc_temp;
}


int16_t calculate_in_temperature(uint16_t adc){//kty 81-110
	//t=((840*adc)/128-1557)/10
	volatile int32_t adc_temp = 840 * adc;
	adc_temp = adc_temp / 128;
	volatile uint16_t cal_offset = 1429;
	cal_offset += eeprom_read_byte(&cal_in_temperature); // 1429 - 1684
	adc_temp = adc_temp - cal_offset;//((1557 * eeprom_read_byte(&cal_ambient_temperature))/128);
	adc_temp = adc_temp / 10;
	return (int16_t) adc_temp;
}


int16_t calculate_oil_temperature(uint16_t adc){//oil temp KTY19-6
	//t=((1715*adc)/256-1560)/10 alt
	//t=((6867 * adc)/1024-1560)/10 neu
	volatile int32_t adc_temp = 6867 * adc;
	adc_temp = adc_temp / 1024;
	volatile uint16_t cal_offset = 1432;
	cal_offset += eeprom_read_byte(&cal_oil_temperature); // 1432 - 1687
	adc_temp = adc_temp - cal_offset;
	adc_temp = adc_temp / 10;
	return (int16_t) adc_temp;
}

int16_t calculate_gearbox_temperature(uint16_t adc){//oil temp KTY19-6
	//t=((1715*adc)/256-1560)/10 alt
	//t=((6867 * adc)/1024-1560)/10 neu
	volatile int32_t adc_temp = 6867 * adc;
	adc_temp = adc_temp / 1024;
	volatile uint16_t cal_offset = 1432;
	cal_offset += eeprom_read_byte(&cal_gearbox_temperature); // 1432 - 1687
	adc_temp = adc_temp - cal_offset;
	adc_temp = adc_temp / 10;
	return (int16_t) adc_temp;
}

void calculate_averages(void){
	//calculate averages occurs every second
	avg_cnt++;
	avg_cnt_start++;
	if(rpm){
		//if(can_status & (1<<ID280) && can_status & (1<<ID288) && can_status & (1<<ID480)){
		speed_sum += speed[CUR];
		cons_sum += (uint16_t) (cons_l_h[CUR]);
		cons_l_h[AVG] = ((float) (cons_sum) / (avg_cnt));
		speed[AVG] = speed_sum / avg_cnt;
		distance[AVG] = (uint32_t) ((speed[AVG] * driving_time[AVG]) / 36000);
		
		if(speed[AVG] > 0.0){
			cons_l_100km[AVG] = cons_l_h[AVG] * 1000 / speed[AVG];
		}else{
			cons_l_100km[AVG] = 99.9;
		}

		if(cons_l_100km[AVG] > 99.9){
			cons_l_100km[AVG] = 99.9;
		}
		
		cons_km_l[AVG] = speed[AVG] / cons_l_h[AVG];
		
		speed_sum_start += speed[CUR];
		cons_sum_start += (uint16_t) (cons_l_h[CUR]);
		cons_l_h_start = ((float) (cons_sum_start) / (avg_cnt_start));
		speed_start = speed_sum_start / avg_cnt_start;
		distance_start = (uint32_t) ((speed_start * driving_time_start) / 36000);

		cons_km_l_start = speed_start / cons_l_h_start;

		if(speed_start > 0.0){
			cons_l_100km_start = cons_l_h_start * 1000 / speed_start;
		}else{
			cons_l_100km_start = 99.9;
		}
		if(cons_l_100km_start > 99.9){
			cons_l_100km_start = 99.9;
		}
		
		if(can_status & (1<<ID320) || can_status & (1<<ID420)){// gauges are connected via can bus - read fuel and calculate range
			range[AVG] = (fuel * 100) / cons_l_100km[AVG];
			range[CUR] = (fuel * 100) / cons_l_100km[CUR];
		}
		//can_status = 0;
	}
	
}


void calculate_consumption(uint16_t delta_ul, uint32_t delta_ms){
#if 1
	if(can_speed_cnt){
		speed[CUR] = ((10 * can_speed_sum * eeprom_read_byte(&cal_speed)) / (can_speed_cnt) / 128);
		if(speed[CUR] < 0) speed[CUR] = 0;
		can_speed_sum = 0;
		can_speed_cnt = 0;
	}else{
		speed[CUR] = 0;
	}
	
	cons_l_h[CUR] = delta_ul * 3.6 * eeprom_read_byte(&cal_consumption);
	cons_l_h[CUR] /= delta_ms ;
	cons_l_h[CUR] /= 128;
#else
	cons_l_h[CUR] = 1.2;
	//if(cons_l_h[CUR] > 15) cons_l_h[CUR] = 0.0;
	speed[CUR] -= 1;
	if(speed[CUR] < 0) speed[CUR] = 0;
	rpm = 678;
	//if(speed[CUR] < 250 || speed[CUR] > 921) speed[CUR] = 921;
	
#endif
	if (speed[CUR]>9){
		cons_l_100km[CUR] = (cons_l_h[CUR] * 1000.0)/(speed[CUR]);
	}else{
		cons_l_100km[CUR] = cons_l_h[CUR];
	}
	cons_km_l[CUR] = speed[CUR] / cons_l_h[CUR];
	
}

void stringcopy(char* source, char* destination, uint8_t length){
	uint8_t i;
	for(i=0;i<length;i++){
		destination[i] = source[i];
	}
}