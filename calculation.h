/*
 * calculation.h
 *
 * Created: 15.10.2014 15:05:57
 *  Author: Hubert
 */ 
#include <avr/io.h>
#include <avr/eeprom.h>
#ifndef CALCULATION_H_
#define CALCULATION_H_


typedef struct vol_val{
	int8_t integer;
	uint8_t fraction;
}voltage_value_t;

voltage_value_t calculate_voltage(uint16_t adc);
void sprint_voltage(char* str,voltage_value_t voltage);
void sprint_voltage_precision(char* str,voltage_value_t voltage, uint8_t prec);
void sprint_temperature(char* str,int16_t temperature);
void uint8_to_string(char* str, uint8_t integer);
void uint16_to_string(char* str, uint16_t integer);
void int16_to_string(char* str, int16_t integer);
int16_t calculate_in_temperature(uint16_t adc);
int16_t calculate_ambient_temperature(uint16_t adc);
int16_t calculate_oil_temperature(uint16_t adc);
int16_t calculate_gearbox_temperature(uint16_t adc);
void stringcopy(char* source, char* destination, uint8_t length);
void sprint_cur_speed(char* str, uint16_t integer);
void sprint_avg_speed(char* str, uint16_t integer);
void sprint_float(char* str, float _float);
void calculate_consumption(uint16_t delta_ul, uint32_t delta_ms);
void calculate_averages(void);
void sprint_distance(char* str, uint64_t distance);
uint32_t round_distance(uint32_t distance);
#endif /* CALCULATION_H_ */