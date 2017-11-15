/*
 * display_task.c
 *
 * Created: 29.06.2017 09:46:23
 *  Author: robert.pohlink
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <string.h>
#include "dog_display.h"
#include "dog_symbol.h"
#include "CAN-MFA.h"
#include "forum.h"
#include "tuer.h"
#include "navigation.h"
#include "display_task.h"
 
 
 void fill_str(char* str, char element, uint8_t length){
	while(length--){
		*str++ = element;
	}
 }

 uint8_t get_text_length(char* text, uint8_t max_len){
	 uint8_t len = 0;
	 for(len = 0; len < max_len; len++){
		 if(text[len] == 0){
			 return len;
		 }
	 }
	 return len;
 }

void display_navi(void){
	position_t position = NEW_POSITION(2,16);
	const uint8_t str_length = 1 + ((COLUMNS - position.column - 48) / 8);
	uint8_t line = 0;
	uint8_t symbol_width_index = 0;
	uint8_t column = 0;

	char str0[] = {0,0,0,0,0,0,0,0,0,0,0};
	char str1[] = {0,0,0,0,0,0,0,0,0,0,0};

	fill_str(str0, ' ', str_length);
	fill_str(str1, ' ', str_length);
	for(line=0;line<6;line++){
		dog_set_position(line+position.page, 0);
		for(column=0; column<position.column;column++){
			dog_transmit_data(0x00);
		}
		for(symbol_width_index=0;symbol_width_index<48; symbol_width_index++){

			if(navigation_status == status_routing || navigation_status == status_recalculating){
				dog_transmit_data(pgm_read_byte(&(nav_48x48[navigation_next_turn][line*48 + symbol_width_index])));
				navi_old = navigation_next_turn;
			}else /*if(navigation_status != status_invalid)*/{
				dog_transmit_data(pgm_read_byte(&(nav_status_48x48[navigation_status][line*48 + symbol_width_index])));
			}
			
		}
		dog_transmit_data(0x00);
		dog_transmit_data(0x00);
		column=position.column + 48;
		
	}

	if(mfa.mode == CUR && speed[CUR] < 100){
		sprint_float(&str0[0],cons_l_h[mfa.mode]);
		str0[5] = ' ';
		str0[6] = CONS_PER_HOUR;
		str0[7] = ' ';
		}else{
		sprint_float(&str0[0],cons_l_100km[mfa.mode]);
		str0[5] = CONS;
		str0[6] = CONS + 1;
		str0[7] = CONS + 2;
	}
	
	if(mfa.mode == CUR){
		str0[0] = ' ';
		}else{
		str0[0] = 0x9D;
	}
	
	if(ambient_temperature < AMBIENT_FROST_TEMP){
		str1[1] = FROST;
		str1[1] = FROST + 1;
	}
	
	sprint_temperature(&str1[3],ambient_temperature);
	str1[6] = CENTIGRADE;

	dog_write_mid_strings(NEW_POSITION(position.page,position.column + 48), str0, str1);
	fill_str(str0, ' ', str_length);
	fill_str(str1, ' ', str_length);
	if(distance_to_next_turn<65000){
		sprint_distance(&str1[1],round_distance(distance_to_next_turn));
	}else{
		sprint_temperature(&str1[1], 255);
	}
	dog_write_mid_strings(NEW_POSITION(position.page+3,position.column + 48), str0, str1);

	
	
}

void display_settings(void){

	uint8_t line = display_mode & ~(1<<SETTINGS);
	
	switch(line){
		case 0:{
			break;
		}
		default:{
			display_mode &= ~(1<<SETTINGS);
			break;
		}
	}
}

void display_small_text(void){
	// vier Werte pro screen
	switch(display_value[SMALL_TEXT]){
		case CAN_VALUES:{
			if(can_mode == NO_CAN){
				display_value[SMALL_TEXT]++;
			}
			// speed, cons, rpm, water
			//				 "0123456789012345"
			char can_line2[22] = "                ";
			//				 "0123456789012345"
			char can_line3[22] = "                ";
			char can_line4[22] = "                ";
			char can_line5[22] = "                ";
			
			#if 1
			can_line2[11] = KMH;
			can_line2[12] = KMH + 1;
			can_line3[4] = ENGT;
			can_line3[5] = ENGT + 1;
			sprint_temperature(&can_line3[6],engine_temperature>25?engine_temperature:200);
			can_line3[9] = CENTIGRADE;
			uint16_to_string(&can_line4[5], rpm);
			can_line4[10] = RPM;
			can_line4[11] = RPM + 1;
			if(mfa.mode == CUR && speed[CUR] < 100){
				sprint_float(&can_line5[5],cons_l_h[mfa.mode]);
				can_line5[11] = CONS_PER_HOUR;
				can_line5[10] = ' ';
				can_line5[12] = ' ';
			}else{
				sprint_float(&can_line5[5],cons_l_100km[mfa.mode]);
				can_line5[10] = CONS;
				can_line5[11] = CONS + 1;
				can_line5[12] = CONS + 2;
			}
			
			if(mfa.mode == CUR){
				can_line2[4] = ' ';
				can_line5[4] = ' ';
				sprint_cur_speed(&can_line2[7],speed[mfa.mode]);
			}else{
				can_line2[4] = 0x9D;
				can_line5[4] = 0x9D;
				sprint_avg_speed(&can_line2[6],speed[mfa.mode]);
			}
			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
			
			#else
			
			can_line2[6] = KMH;
			can_line2[7] = KMH + 1;
			if(mfa.mode == CUR){
				can_line2[0] = ' ';
				can_line5[0] = ' ';
				sprint_cur_speed(&can_line2[2],speed[mfa.mode]);
			}else{
				can_line2[0] = 0x9D;
				can_line5[0] = 0x9D;
				sprint_avg_speed(&can_line2[1],speed[mfa.mode]);
			}
			sprintf(&can_line2[9], "%06X ", speed_sum);
			
			if(mfa.mode == CUR && speed[CUR] < 100){
				sprint_float(&can_line5[1],cons_l_h[mfa.mode]);
				can_line5[7] = CONS_PER_HOUR;
				can_line5[6] = ' ';
				can_line5[8] = ' ';
			}else{
				sprint_float(&can_line5[1],cons_l_100km[mfa.mode]);
				can_line5[6] = CONS;
				can_line5[7] = CONS + 1;
				can_line5[8] = CONS + 2;
			}
			
			sprint_float(&can_line5[10], cons_l_h[mfa.mode]);
			can_line5[15] = CONS_PER_HOUR;
			
			can_line3[0] = ENGT;
			can_line3[1] = ENGT + 1;
			sprint_temperature(&can_line3[2],engine_temperature);
			can_line3[5] = CENTIGRADE;
			
			uint16_to_string(&can_line3[9], rpm);
			can_line3[14] = RPM;
			can_line3[15] = RPM + 1;
			
			sprintf(can_line4, "v:%05d  %06X   ", old_val, cons_sum);
			
			//can_line4[0] = 'o';
			//int16_to_string(&can_line4[1], old_val);
			//can_line4[7] = 'n';
			//int16_to_string(&can_line4[8], new_val);
			//can_line4[10] = RPM;
			//can_line4[11] = RPM + 1;

			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
			
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
			#endif
			break;
		}
		case CAN_VALUES2:{
			if(can_mode == NO_CAN){
				display_value[SMALL_TEXT]++;
			}
			// status, gra,
			//				 "0123456789012345"
			char can_line2[22] = "                 ";
			//				     "01234567890123456"
			char can_line3[22] = "                 ";
			char can_line4[22] = "                 ";
			char can_line5[22] = "                 ";

			sprint_cur_speed(&can_line2[7],(gra_speed * 10 * eeprom_read_byte(&cal_speed)) / 128);
			can_line2[11] = KMH;
			can_line2[12] = KMH + 1;

			sprint_cur_speed(&can_line3[7],speed[CUR] );
			can_line3[11] = KMH;
			can_line3[12] = KMH + 1;
			
			can_line4[4] = 'L';
			sprint_temperature(&can_line4[6],pedal_position);
			can_line4[9] = '%'; //0xF8;
             
								//	012  345678  90123456
			sprintf(&can_line5[0], "  0x%02X  0x%02X     ", eng_status0, eng_status1 );
			
			dog_write_mid_strings(NEW_POSITION(2,0),can_line2, can_line5);
			dog_write_mid_strings(NEW_POSITION(5,0),can_line4, can_line3);
			
			break;
		}
		case ADC_VALUES:{
			if(can_mode == NO_CAN){
				
			}else{
				if(starterbat.integer == 0 && zweitbat.integer == 0 && (oil_temperature > 300 || oil_temperature < -50) && (ambient_temperature > 200 || ambient_temperature <-50) && v_solar_plus.integer == 0 && v_solar_minus.integer == 0 && (gearbox_temperature > 300 || gearbox_temperature < -50) && (in_temperature > 200 || in_temperature <-50)){
					display_value[SMALL_TEXT]++;
					break;
				}
				if(mfa.mode){
					// spg1 spg2 oil aussentenp
						
					//				 "0123456789012345"
					//				 "   BB1 12,50V   "
					char adc_line2[16] = "                ";
					//				 "0123456789012345"
					//				 "   BB2 12,10V   "
					char adc_line3[16] = "                ";
					//				 "0123456789012345"
					//				 "    öT 103GC    "
					char adc_line4[16] = "                ";
					//				 "0123456789012345"
					//				 "    aT -03GC    "
					char adc_line5[16] = "                ";
					adc_line2[3] = BAT;
					adc_line2[4] = BAT+1;
					adc_line2[5] = '1';
					sprint_voltage(&adc_line2[6], starterbat);
					adc_line2[12] = 'V';
					adc_line3[3] = BAT;
					adc_line3[4] = BAT+1;
					adc_line3[5] = '2';
					sprint_voltage(&adc_line3[6], zweitbat);
					adc_line3[12] = 'V';
						
					adc_line4[4] = OILT;
					adc_line4[5] = OILT + 1;
					sprint_temperature(&adc_line4[7],oil_temperature);
					adc_line4[10] = CENTIGRADE;
					if(ambient_temperature < AMBIENT_FROST_TEMP){
						adc_line5[4] = FROST;
						adc_line5[5] = FROST + 1;
					}
						
					sprint_temperature(&adc_line5[7],ambient_temperature);
					adc_line5[10] = CENTIGRADE;
					dog_write_mid_strings(NEW_POSITION(2,0),adc_line2, adc_line3);
						
					dog_write_mid_strings(NEW_POSITION(5,0),adc_line4, adc_line5);
				
				//*/
				}else{
					char adc_line2[16] = "                ";
					//				 "0123456789012345"
					//				 "   BB2 12,10V   "
					char adc_line3[16] = "                ";
					//				 "0123456789012345"
					//				 "    gT 103GC    "
					char adc_line4[16] = "                ";
					//				 "0123456789012345"
					//				 "    iT -03GC    "
					char adc_line5[16] = "                ";
					adc_line2[3] = BAT;
					adc_line2[4] = BAT+1;
					adc_line2[5] = '3';
					sprint_voltage(&adc_line2[6], v_solar_plus);
					adc_line2[12] = 'V';
					adc_line3[3] = BAT;
					adc_line3[4] = BAT+1;
					adc_line3[5] = '4';
					sprint_voltage(&adc_line3[6], v_solar_minus);
					adc_line3[12] = 'V';
				
					adc_line4[3] = GEARBOXT;
					adc_line4[4] = GEARBOXT + 1;
					adc_line4[5] = 'C';
					sprint_temperature(&adc_line4[7],gearbox_temperature);
					adc_line4[10] = CENTIGRADE;
				
					adc_line5[4] = INT;
					adc_line5[5] = INT + 1;
					sprint_temperature(&adc_line5[7],in_temperature);
					adc_line5[10] = CENTIGRADE;
					dog_write_mid_strings(NEW_POSITION(2,0),adc_line2, adc_line3);
				
					dog_write_mid_strings(NEW_POSITION(5,0),adc_line4, adc_line5);
				}
			}
			break;
			}
			//*/
			case STANDARD_VALUES:{
				/*	0123456789012345
					RADIO TEXT
					----------------
					  ab reset/start
					 42kmh  12,3lkm
					  6,6lh  123gC
					 1234km  12:34
				*/
				char line1[17] = "                "; 
				char line2[17] = "                ";
				char line3[17] = "                ";
				char line4[17] = "                ";
				dog_set_position(2,0);
				//							"                ":"                ";
				strcpy(line1, mfa.mode==CUR?"    ab Reset    ":"    ab Start    ");
				
				sprint_cur_speed(&line2[1], mfa.mode==CUR?speed[AVG]:speed_start);
				line2[4] = KMH;
				line2[5] = KMH + 1;
				
				sprint_float(&line2[7], mfa.mode==CUR?cons_l_100km[AVG]:cons_l_100km_start);
				line2[12] = CONS;
				line2[13] = CONS + 1;
				line2[14] = CONS + 2;
				
				dog_write_mid_strings(NEW_POSITION(2,0), line1,line2);
				
				sprint_float(&line3[9], mfa.mode==CUR?cons_l_h[AVG]:cons_l_h_start);
				line3[14] = CONS_PER_HOUR;
	
				sprint_temperature(&line3[3],ambient_temperature);
				line3[6] = CENTIGRADE;
				
				if(ambient_temperature < AMBIENT_FROST_TEMP){
					line3[1] = FROST;
					line3[2] = FROST + 1;
				}
				
				uint16_to_string(&line4[1], mfa.mode==CUR?distance[AVG]:distance_start);
				
				line4[6] = 'k';
				line4[7] = 'm';
				
				uint8_t dt_hour;
				uint8_t dt_minute;
				if(mfa.mode == CUR){
					dt_hour = (driving_time[AVG] / 3600);
					dt_minute = (driving_time[AVG] % 3600) / 60;
					
					uint8_to_string(&line4[9], dt_hour);
					uint8_to_string(&line4[12], dt_minute);
				}else{
					dt_hour = (driving_time_start / 3600);
					dt_minute = (driving_time_start % 3600) / 60;
					
					uint8_to_string(&line4[9], dt_hour);
					uint8_to_string(&line4[12], dt_minute);
				}
				
				line4[12] = ':';
				if(dt_hour < 10){
					line4[10] = '0';
				}
				if(dt_minute < 10){
					line4[13] = '0';
				}
				
				dog_write_mid_strings(NEW_POSITION(5,0), line3,line4);
				break;
			}
			case MIN_MAX_VALUES:{
				// speed, rpm, temperature (eng/oil/out/gearbox)
				/*	0123456789012345
					RADIO TEXT
					----------------
					  MAX    123gC
					 42kmh  1234rpm
					  123gC  123gC
					  123gC  123gC
				*/
				char line1[17] = "                "; 
				char line2[17] = "                ";
				char line3[17] = "                ";
				char line4[17] = "                ";

				dog_set_position(2,0);
				//							"                ":"                ";
				strcpy(line1, mfa.mode==CUR?" max            ":" min            ");
				line1[14] = CENTIGRADE;

				sprint_temperature(&line1[11],mfa.mode==CUR?max_ambient_temp:min_ambient_temp);
				
				sprint_cur_speed(&line2[1],  mfa.mode==CUR?max_speed:0);
				line2[4] = KMH;
				line2[5] = KMH + 1;
				
				uint16_to_string(&line2[7],  mfa.mode==CUR?max_rpm:0);
				
				line2[13] = RPM;
				line2[14] = RPM + 1;

				dog_write_mid_strings(NEW_POSITION(2,0), line2,line1);
				line3[1] = GEARBOXT;
				line3[2] = GEARBOXT + 1;
				sprint_temperature(&line3[3],mfa.mode==CUR?max_gearbox_temp:min_gearbox_temp);
				line3[6] = CENTIGRADE;
				
				line3[9] = INT;
				line3[10] = INT + 1;
				sprint_temperature(&line4[11],mfa.mode==CUR?max_in_temp:min_in_temp);
				line3[14] = CENTIGRADE;
				
				line4[1] = ENGT;
				line4[2] = ENGT + 1;
				sprint_temperature(&line4[3],mfa.mode==CUR?max_engine_temp:min_engine_temp);
				line4[6] = CENTIGRADE;
				
				line4[9] = OILT;
				line4[10] = OILT + 1;
				sprint_temperature(&line4[11],mfa.mode==CUR?max_oil_temp:min_oil_temp);
				line4[14] = CENTIGRADE;
				
				dog_write_mid_strings(NEW_POSITION(5,0), line3,line4);
				break;
			}
			case TEMPERATURE_VALUES:{
				// speed, rpm, temperature (eng/oil/out/gearbox)
				/*	0123456789012345
					RADIO TEXT
					----------------
					  MAX    123gC
					 42kmh  1234rpm
					  123gC  123gC
					  123gC  123gC
				*/
				char line1[17] = "                "; 
				char line2[17] = "                ";
				char line3[17] = "                ";
				char line4[17] = "                ";

				dog_set_position(2,0);

				strcpy(line1, " cur            ");
				line1[14] = CENTIGRADE;

				sprint_temperature(&line1[11], ambient_temperature);
				
				sprint_cur_speed(&line2[1], (uint16_t) speed[CUR]);
				line2[4] = KMH;
				line2[5] = KMH + 1;
				
				uint16_to_string(&line2[7], rpm);
				
				line2[13] = RPM;
				line2[14] = RPM + 1;

				dog_write_mid_strings(NEW_POSITION(2,0), line2,line1);
				line3[1] = GEARBOXT;
				line3[2] = GEARBOXT + 1;
				sprint_temperature(&line3[3],gearbox_temperature);
				line3[6] = CENTIGRADE;
				
				line3[9] = INT;
				line3[10] = INT + 1;
				sprint_temperature(&line4[11],in_temperature);
				line3[14] = CENTIGRADE;
				
				line4[1] = ENGT;
				line4[2] = ENGT + 1;
				sprint_temperature(&line4[3], engine_temperature);
				line4[6] = CENTIGRADE;
				
				line4[9] = OILT;
				line4[10] = OILT + 1;
				sprint_temperature(&line4[11], oil_temperature);
				line4[14] = CENTIGRADE;
				
				dog_write_mid_strings(NEW_POSITION(5,0), line3,line4);
				break;
			}
			case STARTSTOP:{
				if(can_mode==NO_CAN){
					display_value[SMALL_TEXT]++;
					break;
				}
				/*	0123456789012345
		     Status
					----------------
					123kmh  1200upm  	
					 XXX  XXX  XXX  
					et85gc	12gC
					AC:OFF	12,34V
				*/  
				/*
				switch(status){
					case START:{		//	012345678901
						strcpy(status_str, "   START    ");
						break;
					}
					case STOP:{
						strcpy(status_str, "   STOP     ");
						break;
					}
					case FAHRT:{
						strcpy(status_str, "   FAHRT    ");
						break;
					}
					case MOTOR_AUS:{
						strcpy(status_str, " MOTOR AUS  ");
						break;
					}
					case DEAKTIVIERT:{
						strcpy(status_str, "DEAKTIVIERT ");
						break;
					}
				}
				*/
				char line1[17] = "                "; 
				char line2[17] = "                ";
				char line3[17] = "                ";
				char line4[17] = "                ";
				dog_set_position(2,0);
				
				sprint_cur_speed(&line1[1], speed[CUR]);
				line1[4] = KMH;
				line1[5] = KMH + 1;
				
				uint16_to_string(&line1[7], rpm);
				line1[12] = RPM;
				line1[13] = RPM + 1;
	
				//	 01234567890123456
				//	" XXX  XXX  XXX  "
	
				if(!(eng_status0 & CLUTCH)){
					line2[1] = 'X';
					line2[2] = 'X';
					line2[3] = 'X';
				}else{
					line2[1] = '-';
					line2[2] = '-';
					line2[3] = '-';
				}
	
				if(eng_status1 & BRAKE){
					line2[6] = 'X';
					line2[7] = 'X';
					line2[8] = 'X';
				}else{
					line2[6] = '-';
					line2[7] = '-';
					line2[8] = '-';
				}
	
				if(!(eng_status0 & THROTTLE)){
					line2[11] = 'X';
					line2[12] = 'X';
					line2[13] = 'X';
				}else{
					line2[11] = '-';
					line2[12] = '-';
					line2[13] = '-';
				}

				dog_write_mid_strings(NEW_POSITION(2,0), line1,line2);
	
				line3[1] = ENGT;
				line3[2] = ENGT + 1;
				sprint_temperature(&line3[3],engine_temperature>25?engine_temperature:200);
				line3[6] = CENTIGRADE;

				sprint_temperature(&line3[10],ambient_temperature);
				line3[13] = CENTIGRADE;

				if(ambient_temperature < 4){
					line3[8] = FROST;
					line3[9] = FROST + 1;
				}
				//				01234567890123456
				//				AC: OFF   12,34V 
				sprintf(line4, " AC: %s %2i,%02iV  ", (eng_status1 & AC)?"ON ":"OFF", starterbat.integer, starterbat.fraction);
				dog_write_mid_strings(NEW_POSITION(5,0), line3,line4);
				break;
			}
			default:{
				display_value[SMALL_TEXT]=0;
				break;
			}
		}
	}		


void display_med_row(volatile uint8_t* dv, uint8_t page, uint8_t row){
	uint8_t i;
	char str[12] = "            ";
	uint8_t display_value = *dv;
	if((display_mode == MED_TEXT_TOP && page < 4) || (display_mode == MED_TEXT_BOT && page > 3)){
		str[0] = '>';
		str[9] = '<';
	}
	
	switch(display_value){
		case VAL_CUR_SPEED:{

			sprint_cur_speed(&str[3], speed[CUR]);
			str[7] = KMH;
			str[8] = KMH+1;

			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_AVG_SPEED:{
			str[1] = 0x9D;
			sprint_avg_speed(&str[2], speed[AVG]);
			str[7] = KMH;
			str[8] = KMH+1;
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_CUR_CONS:{
			if(speed[CUR] < 100){
				sprint_float(&str[2], cons_l_h[CUR]);
				str[7] = ' ';
				str[8] = CONS_PER_HOUR;
			}else{
				sprint_float(&str[2], cons_l_100km[CUR]);
				str[7] = CONS;
				str[8] = CONS+1;
			}
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_AVG_CONS:{
			str[1] = 0x9D;
			sprint_float(&str[2], cons_l_100km[AVG]);
			str[7] = CONS;
			str[8] = CONS+1;
				
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		
		case VAL_CONS_START:{
			str[1] = 's';
			sprint_float(&str[2], cons_l_100km_start);
			str[7] = CONS;
			str[8] = CONS+1;
			
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		
		case VAL_CONS_KM_L_CUR:{

			sprint_float(&str[2], cons_km_l[CUR]);
			str[7] = CONS_KM_L;
			str[8] = CONS_KM_L+1;
			
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		
		case VAL_CONS_KM_L_AVG:{
			str[1] = 0x9D;
			sprint_float(&str[2], cons_km_l[AVG]);
			str[7] = CONS_KM_L;
			str[8] = CONS_KM_L+1;
			
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		
		case VAL_CONS_KM_L_START:{
			str[1] = 's';
			sprint_float(&str[2], cons_km_l_start);
			str[7] = CONS_KM_L;
			str[8] = CONS_KM_L+1;
			
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12+6,row),str[i]);
				str[i] = ' ';
			}
			break;
		}

		//
		case VAL_VOLTA:{//+
			//*
			if(starterbat.integer == 0){
				display_value++;
				break;
			}
			//*/
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'1');
			sprint_voltage(&str[0], starterbat);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_VOLTB:{
			//*
			if(zweitbat.integer == 0){
				display_value++;
				break;
			}
			//*/
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'2');
			sprint_voltage(&str[0], zweitbat);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_VOLTC:{
			if(v_solar_plus.integer == 0){
				display_value++;
				break;
			}
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'3');
			sprint_voltage(&str[0], v_solar_plus);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_VOLTD:{
			if(v_solar_minus.integer == 0){
				display_value++;
				break;
			}
			dog_write_big_digit(NEW__POSITION(page,0,row),' ');
			dog_write_numbered_bat_symbol(NEW__POSITION(page,12,row),'4');
			sprint_voltage(&str[0], v_solar_minus);
			str[6] = 'V';
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,36+i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_OIL:{
			if(oil_temperature > 150 || oil_temperature < -50){
				display_value++;
				break;
			}
			
			str[1] = OILT;
			
			sprint_temperature(&str[4], oil_temperature);
			str[2] = OILT+1;
			str[8] = CENTIGRADE;
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_ENGT:{
			if(oil_temperature > 150 || oil_temperature < 25){
				display_value++;
				break;
			}
			str[1] = ENGT;
				
			sprint_temperature(&str[4], engine_temperature>25?engine_temperature:200);
			str[2] = ENGT+1;
			str[8] = CENTIGRADE;
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
					
			}
			break;
		}
		case VAL_AMBIENT:{
			//if(ambient_temperature < 150 && ambient_temperature > -50){
				if(ambient_temperature < AMBIENT_FROST_TEMP){
					str[1] = FROST;
					str[2] = FROST +1;
				}
				sprint_temperature(&str[4], ambient_temperature);
				str[8] = CENTIGRADE;
				for(i=0; i<12; i++){
					dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
					str[i] = ' ';
				}
			/*		
			}else{
				display_value++;
				break;
			}
			//*/
			break;
		}
		case VAL_GEARBXT:{
			//*
			if(gearbox_temperature > 300 && gearbox_temperature < -50){
				display_value++;
				break;
			}
			//*/
			str[1] = 'G';
			str[2] = 'T';
			
			sprint_temperature(&str[4], gearbox_temperature);

			str[8] = CENTIGRADE;

			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		case VAL_INT:{
			//*
			if(in_temperature > 300 && in_temperature < -50){
				display_value++;
				break;
			}
			//*/
			str[1] = 'i';
			str[2] = 'n';
			
			sprint_temperature(&str[4], in_temperature);

			str[8] = CENTIGRADE;

			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		//*
		case VAL_RPM:{
			//	0123456789012
			//	  12345 rpm
			uint16_to_string(&str[2], rpm);
			str[8] = RPM;
			str[9] = RPM + 1;
			for(i=0; i<12; i++){
				dog_write_big_digit(NEW__POSITION(page,i*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}

		case VAL_TIME:{

			// 0123456789012
			//      01234567
			//       12:34
			char time[9] = "        ";
			uint8_t dt_hour, dt_minute;
			if(mfa.mode == CUR){
				dt_hour = (driving_time[AVG] / 3600);
				dt_minute = (driving_time[AVG] % 3600) / 60;
				
				uint8_to_string(&time[0], dt_hour);
				uint8_to_string(&time[3], dt_minute);
				dog_set_page(page);
				dog_write_small_string("    ab    ");
				dog_set_page(page+1);
				dog_write_small_string("   Reset  ");
			}else{
				dt_hour = (driving_time_start / 3600);
				dt_minute = (driving_time_start % 3600) / 60;
				
				uint8_to_string(&time[0], dt_hour);
				uint8_to_string(&time[3], dt_minute);
				dog_set_page(page);
				dog_write_small_string("    ab    ");
				dog_set_page(page+1);
				dog_write_small_string("   Start  ");
			}
			str[3] = ':';
			if(dt_hour < 10){
				str[1] = '0';
			}
			if(dt_minute < 10){
				str[4] = '0';
			}
			for(i=0; i<7; i++){
				dog_write_big_digit(NEW__POSITION(page,(i+5)*12,row),str[i]);
				str[i] = ' ';
			}
			break;
		}
		#if 0
		case VAL_MANIFOLD:{
			display_value++;
			break;
			/*
			sprintf(str, " %lu ! ", (unsigned long) driving_time[mfa.mode]);
			dog_set_position(2,0);
			dog_write_small_string(str);
			dog_set_position(3,0);
			dog_write_small_string("Manifold");
			break;
			*/
		}

		case VAL_FUEL:{
			display_value++;
			/*
			sprintf(str, "%lu ! ", (unsigned long) driving_time[mfa.mode]);
			dog_set_position(3,0);
			dog_write_small_string(str);
			dog_set_position(4,0);
			dog_write_small_string("Fuel");
			*/
			break;
		}
		case VAL_RANGE:{
			display_value++;
			break;
		}
		#endif
		//*/
		default:{
			display_value=0;
			break;
		}
	}
	*dv = display_value;
}

void display_med_text(void){
	
	/*
	upper half: displaymode = MED_TOP
	lower half: displaymode = MED_BOT
	*/
	//*
	if(display_mode == MED_TEXT_TOP){
		if(old_display_mode != MED_TEXT_TOP){
			display_value[MED_TEXT_TOP] = display_value_top;
		}else{
			display_value_top = display_value[MED_TEXT_TOP];
		}
	}else if(display_mode == MED_TEXT_BOT){
		if(old_display_mode != MED_TEXT_BOT){
			display_value[MED_TEXT_BOT] = display_value_bot;
		}else{
			display_value_bot = display_value[MED_TEXT_BOT];
		}
	}
	//*/
	uint8_t page = 2;
	uint8_t row = 4;
	display_med_row(&display_value_top, page, row);
	//display_med_row(&display_value[MED_TEXT_TOP], page, row);
	page = 5;
	row = 6;
	display_med_row(&display_value_bot, page, row);
	//display_med_row(&display_value[MED_TEXT_BOT], page, row);
	
}
		
void display_tuer_open(void){
	uint8_t a,b;
	for(a=0;a<8;a++){
		dog_set_position(a,4);
		for(b=0;b<128; b++){
			dog_transmit_data(pgm_read_byte(&(sym_tuer2[a*128 + b])));
		}
	}
}

void display_tuer_closed(void){
	uint8_t a,b;
	for(a=0;a<8;a++){
		dog_set_position(a,4);
		for(b=0;b<128; b++){
			dog_transmit_data(pgm_read_byte(&(sym_tuer3[a*128 + b])));
		}
	}
}

void generate_can_display_str_byte(char str[17], uint8_t index, volatile uint8_t can_data[8]){
	str[1] = index + '0';
	str[2] = ' ';
	snprintf(&str[3], 5, "0x%02X    ", can_data[index]);
	uint8_to_string(&str[8],can_data[index]);
}

void generate_can_display_str_word(char str[17], uint8_t index, volatile uint8_t can_data[8]){
	str[1] = index + '0';
	str[2] = ' ';
	//rpm = (id280_data[4] + ((id280_data[3]) << 8))>>2;
	uint16_t tmp = (can_data[index] << 8  ) + can_data[index+1];

	snprintf(&str[3], 7, "0x%04X ", tmp);
	uint16_to_string(&str[10],tmp);
}

void display_can_data(void){

	if(can_status){
	//	#if 1
		switch(display_value[CAN_DATA]){
			case 0:{
				if(mfa.mode){
					uint8_t i;
					char id[3] = {0,0,0};
					dog_set_position(2,8);
					dog_write_tiny_string("280");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id280_data[i]);
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(3,8);
					dog_write_tiny_string("288");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id288_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(4,8);
					dog_write_tiny_string("380");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id380_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
					dog_set_position(5,8);
					dog_write_tiny_string("480");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id480_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(6,8);
					dog_write_tiny_string("320");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id320_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(7,8);
					dog_write_tiny_string("420");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id420_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
				}else{
					uint8_t i;
					char id[3] = {0,0,0};
					dog_set_position(2,8);
					dog_write_tiny_string("520");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id520_data[i]);
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(3,8);
					dog_write_tiny_string("666");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id666_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(4,8);
					dog_write_tiny_string("667");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", id667_data[i]);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
					dog_set_position(5,8);
					dog_write_tiny_string("xxx");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", 255);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(6,8);
					dog_write_tiny_string("xxx");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", 255);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
							
					dog_set_position(7,8);
					dog_write_tiny_string("xxx");
							
					for(i=0; i<8; i++){
						sprintf(id, "%02X", 255);
								
						dog_transmit_data(0x00);
						dog_write_small_string(id);
					}
				}
				break;
			}
			case 1:{
				/*
				1: 0xAA  
				
				*/
				//				 01234567890123456
				char str0[17] =	"                 ";
				char str1[17] =	"                 ";
				if(mfa.mode){
					str0[1] = '2';
					str0[2] = '8';
					str0[3] = '0';
					generate_can_display_str_word(str1, 1, id280_data);
				}else{
					generate_can_display_str_byte(str0, 1, id280_data);
					generate_can_display_str_byte(str1, 2, id280_data);
				}
				dog_write_mid_strings(NEW_POSITION(2,0), str0, str1);
				generate_can_display_str_byte(str0, 6, id280_data);
				generate_can_display_str_byte(str1, 7, id280_data);
				dog_write_mid_strings(NEW_POSITION(5,0), str0, str1);
				// check unknown values from ID 280
				// [status] [ 1 ] [ 2 ] [rpm] [rpm] [ pedal_position ] [ 6 ] [ 7 ]
				break;
			}
			case 2:{
				// check unknown values from ID 288
				// [ 0 ] [ eng_temp ] [ status1 ] [ spd ] [spd_gra] [ 5 ] [ 6 ] [ 7 ]
				char str0[17] =	"                 ";
				char str1[17] =	"                 ";
				generate_can_display_str_byte(str0, 0, id288_data);
				generate_can_display_str_byte(str1, 6, id288_data);

				dog_write_mid_strings(NEW_POSITION(2,0), str0, str1);
				if(mfa.mode){
					generate_can_display_str_word(str0, 5, id288_data);
					generate_can_display_str_word(str1, 6, id288_data);
				}else{
					generate_can_display_str_byte(str0, 6, id288_data);
					generate_can_display_str_byte(str1, 7, id288_data);
				}
				dog_write_mid_strings(NEW_POSITION(5,0), str0, str1);
				break;
			}
			case 3:{
				// check unknown values from ID 480
				// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [ - ] [ - ] [ - ] [ - ]
				char str0[17] =	"                 ";
				char str1[17] =	"                 ";
				if(mfa.mode){
					str0[1] = '3';
					str0[2] = '8';
					str0[3] = '0';
					generate_can_display_str_word(str1, 0, id380_data);
				}else{
					generate_can_display_str_byte(str0, 0, id380_data);
					generate_can_display_str_byte(str1, 1, id380_data);
				}

				

				dog_write_mid_strings(NEW_POSITION(2,0), str0, str1);
				if(mfa.mode){
					generate_can_display_str_word(str0, 2, id380_data);
					generate_can_display_str_word(str1, 3, id380_data);
				}else{
					generate_can_display_str_byte(str0, 2, id380_data);
					generate_can_display_str_byte(str1, 3, id380_data);
				}
				dog_write_mid_strings(NEW_POSITION(5,0), str0, str1);
				break;
			}
			case 4:{
				// check unknown values from ID 480
				// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [ - ] [ - ] [ - ] [ - ]
				char str0[17] =	"                 ";
				char str1[17] =	"                 ";
				if(mfa.mode){
					str0[1] = '4';
					str0[2] = '8';
					str0[3] = '0';
					generate_can_display_str_word(str1, 0, id480_data);
					}else{
					generate_can_display_str_byte(str0, 0, id480_data);
					generate_can_display_str_byte(str1, 1, id480_data);
				}

				

				dog_write_mid_strings(NEW_POSITION(2,0), str0, str1);
				if(mfa.mode){
					generate_can_display_str_word(str0, 2, id480_data);
					generate_can_display_str_word(str1, 3, id480_data);
					}else{
					generate_can_display_str_byte(str0, 2, id480_data);
					generate_can_display_str_byte(str1, 3, id480_data);
				}
				dog_write_mid_strings(NEW_POSITION(5,0), str0, str1);
				break;
			}
			/*
			case 4:{
				// check unknown values from ID 480
				// [ 0 ] [ 1 ] [ cons ] [ cons ] [ 4 ] [ 5 ] [ 6 ] [ 7 ]
				break;
			}*/
			default:{
				display_value[CAN_DATA] = 0;
				break;
			}
		}
	}else{
		uint8_t a=0,b=0,c=2;
		for(a=0;a<6;a++){
			dog_set_position(a + c,4);
			for(b=0;b<128; b++){
				dog_transmit_data(pgm_read_byte(&(sym_t4forum_bmp[a*128 + b])));
			}
		}
	}

}

void display_task(){
	/* 0123456789012345 *	* 012345678901234567890 *
	 - ---------------- -	* 
	 * Musiktitel (16)	*	*
	 - ---------------- -	*
	 * Fahrzeit/Strecke *	*
	 * Geschwindigkeit  *	*
	 *		Verbrauch	*	*
	 *		Temperatur	*	*
	 --------------------	*
	 *						*
	 */
	
	
	/* 0123456789012345 *	* 012345678901234567890 *
	 - ---------------- -	* 
	 * Musiktitel (16)	*	*
	 - ---------------- -	*
	 *  Sym1 NAVI Sym2  *	*
	 *  VAL1 NAVI VAL2  *	*
	 *		 NAVI		*	*
	 *		 NAVI		*	*
	 --------------------	*
	 *						*
	 */
	
	if(!(TKML_PIN & (1<<TKML))){
		//				.012345678901.
		char _str[] =	"            ";
		_str[4] = DOOR;
		_str[5] = DOOR + 1;
		_str[6] = DOOR2;
		dog_write_big_string(NEW_POSITION(0,4),_str);
		line_shift_timer = LINE_SHIFT_START;
	}else{
		underlined = 1;
		if(can_mode==CAN){
			dog_write_rotating(NEW_POSITION(0,5),(char*) &radio_text[0], get_text_length((char*) &radio_text[0], sizeof(radio_text)),SIZE8X12,line_shift_timer);
		}else{
			//				0123456789012345
			//				 12,3V BB 12,3V
			char str[17] = "                ";
			sprint_voltage_precision(&str[1], starterbat, 1);
			sprint_voltage_precision(&str[10], zweitbat, 1);
			str[5] = 'V';
			str[14] = 'V';
			str[7] = BAT;
			str[8] = BAT+1;
			dog_write_mid_string(NEW_POSITION(0,0),str);
		}
		underlined = 0;
	}
	
	underlined = 0;

	if(display_mode & (1<<SETTINGS)){
		display_settings();
	}else{
		switch(display_mode){
			case NAVIGATION:{
				if(can_mode == NO_CAN){
					display_mode++;
					break;
				}
				display_navi();
				break;
			}
			case SMALL_TEXT:{
				display_small_text();
				break;
			}
			case MED_TEXT_TOP:{
				display_med_text();
				break;
			}
			case MED_TEXT_BOT:{
				display_med_text();
				break;
			}
			case CAN_DATA:{
				if(can_mode == NO_CAN){
					display_mode++;
					break;
				}
				display_can_data();					
				break;
			}
			default:{
				if(can_mode == NO_CAN){
					display_mode = 1;
				}else{
					display_mode = 0;
				}
				break;
			}
		
		}
	}	
	old_display_mode = display_mode;	

}