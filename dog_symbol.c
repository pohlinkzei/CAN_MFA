/*
 * dog_symbol.c
 *
 * Created: 06.10.2014 11:26:07
 *  Author: Hubert
 */ 
#include "dog_display.h"
#include "dog_symbol.h"
#include "calculation.h"

extern volatile uint8_t display_mode;

void display_navigation_status(position_t position, int8_t status){
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	char distance_str[7] = {0,};
	if(status<0){
		display_mode++;
		return;
	}	

	for(a=0;a<6;a++){
		dog_set_position(a+position.page, 0);
		for(c=0; c<position.column;c++){
			dog_transmit_data(0x00);
		}
			
		for(b=0;b<48; b++){
			dog_transmit_data(pgm_read_byte(&(nav_status_48x48[status][a*48 + b])));
		}

		c=position.column + 48;
					
	}
	c=position.column + 48;
	dog_set_position(position.page, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	dog_set_position(position.page + 1, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}

	dog_set_position(position.page +4, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	dog_set_position(position.page + 5, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	dog_set_position(position.page + 2, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
}

void display_navigation_symbol(position_t position, int8_t next_turn,  uint64_t distance){
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	char distance_str[7] = {0,};
	if(next_turn<0){
		display_mode++;
		return;
	}	
	/*	
	//uint8_t d = 132-(position.column+48);
	if(position.column < 40){
		
	}else if(position.column > 40 && (position.column < )){
	}
	//*/
	for(a=0;a<6;a++){
		dog_set_position(a+position.page, 0);
		for(c=0; c<position.column;c++){
			dog_transmit_data(0x00);
		}
		//dog_set_position(a+position.page, position.column);			
		for(b=0;b<48; b++){
			dog_transmit_data(pgm_read_byte(&(nav_48x48[next_turn][a*48 + b])));
		}
		/*
		//dog_set_position(a+position.page, position.column + 48);
		for(c=position.column + 48; c<132;c++){
			dog_transmit_data(0x00);
		}
		*/
		c=position.column + 48;
					
	}
	c=position.column + 48;
	sprint_distance(distance_str, round_distance(distance));
	dog_set_position(position.page, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	dog_set_position(position.page + 1, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	
	dog_write_mid_string(NEW__POSITION(position.page + 2,position.column + 48,2), distance_str);
	dog_set_position(position.page +4, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	dog_set_position(position.page + 5, position.column + 48);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
	dog_set_position(position.page + 2, position.column + 104);
	for(a=c; a<132; a++){
		dog_transmit_data(0x00);
	}
/*	uint8_t i,j;
	uint8_t temp[6][48];

	for(i=0;i<48;i++){
		temp[0][i]	= pgm_read_byte(&(nav_48x48[next_turn][(i*6)]));
		temp[1][i]	= pgm_read_byte(&(nav_48x48[next_turn][(i*6)+1]));
		temp[2][i]	= pgm_read_byte(&(nav_48x48[next_turn][(i*6)+2]));
		temp[3][i]	= pgm_read_byte(&(nav_48x48[next_turn][(i*6)+3]));
		temp[4][i]	= pgm_read_byte(&(nav_48x48[next_turn][(i*6)+4]));
		temp[5][i]	= pgm_read_byte(&(nav_48x48[next_turn][(i*6)+5]));
	}	

	//dog_set_position(position.page, position.column);
	for(j = 0; j < 6; j++){
		dog_set_position(position.page + j, position.column);
		for(i = 0; i < 48; i++){
			dog_transmit_data(temp[j][i]);
		}
	}	
	*/
}
//*/
