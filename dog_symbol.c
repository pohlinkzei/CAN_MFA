/*
 * dog_symbol.c
 *
 * Created: 06.10.2014 11:26:07
 *  Author: Hubert
 */ 
#include "dog_display.h"
#include "dog_symbol.h"

extern volatile uint8_t display_mode;

void display_navigation_symbol(position_t position, uint8_t next_turn){
	uint8_t i,j;
	uint8_t temp[4][32];

	for(i=0;i<32;i++){
		temp[0][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)]));
		temp[1][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)+1]));
		temp[2][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)+2]));
		temp[3][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)+3]));
	}	

	//dog_set_position(position.page, position.column);
	for(j = 0; j < 4; j++){
		dog_set_position(position.page + j, position.column);
		for(i = 0; i < 32; i++){
			dog_transmit_data(temp[j][i]);
		}
	}	
	
}

void display_navigation_symbol48(position_t position, int8_t next_turn){
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	if(next_turn<0){
		display_mode++;
		return;
	}		
	//uint8_t d = 132-(position.column+48);
	for(a=0;a<6;a++){
		dog_set_position(a+position.page, 0);
		for(c=0; c<position.column;c++){
			dog_transmit_data(0x00);
		}
		//dog_set_position(a+position.page, position.column);			
		for(b=0;b<48; b++){
			
			dog_transmit_data(pgm_read_byte(&(nav_48x48[next_turn][a*48 + b])));
		}
		//dog_set_position(a+position.page, position.column + 48);
		for(c=position.column + 48; c<132;c++){
			dog_transmit_data(0x00);
		}			
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
void print_navigation_symbol(uint8_t fbuf[ROWS][COLUMNS], position_t position, uint8_t next_turn){
	uint8_t i,j;
	uint8_t temp[4][32];
	for(i=0;i<32;i++){
		temp[0][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)]));
		temp[1][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)+1]));
		temp[2][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)+2]));
		temp[3][i]	= pgm_read_byte(&(nav_32x32[next_turn][(i*4)+3]));
	}
	for(j = 0; j < 4; j++){
		for(i = 0; i < 32; i++){
			fbuf[position.page + j][position.column + i] = temp[j][i];
		}
	}			
}


/*
void display_symbol_12x16(position_t position, uint8_t symbol){
	uint8_t i;
	uint8_t temp[2][16];
	//symbol = 0;
	for(i=0; i<16; i++){
		temp[1][i] = pgm_read_byte(&(symbol_12x16[symbol][2*i+1]));
		temp[0][i] = pgm_read_byte(&(symbol_12x16[symbol][2*i]));
	}
	dog_set_position(position.page, position.column);
	for(i=0; i<16; i++){
		dog_transmit_data(temp[0][i]);
	}
	dog_set_position(position.page+1, position.column);
	for(i=0; i<16; i++){
		dog_transmit_data(temp[1][i]);
	}	
	//dog_write_small_digit(symbol + '0');
}

void display_symbol_12x24(position_t position, uint8_t symbol){
	uint8_t i;
	uint8_t temp[3][24] = {{0}};
	uint32_t temprow = 0;
	//symbol = 0;
	
	for(i=0; i<24; i++){
		temp[1][i] = pgm_read_byte(&(symbol_12x24[symbol][2*i+1]));
		temp[0][i] = pgm_read_byte(&(symbol_12x24[symbol][2*i]));
	}
	if(position.row){
		for(i=0; i<24; i++){
			temprow = 0;
			temprow = ((temp[0][i]) + ((uint16_t) temp[1][i] << 8) + ((uint32_t) temp[2][i] << 16));
			temprow = temprow << position.row;
			temp[0][i] = (uint8_t) ((temprow) & 0x000000FF);
			temp[1][i] = (uint8_t) ((temprow>>8) & 0x000000FF);
			temp[2][i] = (uint8_t) ((temprow>>16) & 0x000000FF);	
		}	
		
		dog_set_position(position.page, position.column);
		for(i=0; i<24; i++){
			dog_transmit_data(temp[0][i]);
		}
		dog_set_position(position.page+1, position.column);
		for(i=0; i<24; i++){
			dog_transmit_data(temp[1][i]);
		}
		dog_set_position(position.page+2, position.column);
		for(i=0; i<24; i++){
			dog_transmit_data(temp[2][i]);
		}		
	}else{
		dog_set_position(position.page, position.column);
		for(i=0; i<24; i++){
			dog_transmit_data(temp[0][i]);
		}
		dog_set_position(position.page+1, position.column);
		for(i=0; i<24; i++){
			dog_transmit_data(temp[1][i]);
		}	
	}
	
	//dog_write_small_digit(symbol + '0');
}


void print_symbol_12x16(uint8_t fbuf[ROWS][COLUMNS], position_t position, uint8_t symbol){
	uint8_t i;
	for(i=0; i<16; i++){
		fbuf[position.page][position.column + i] = pgm_read_byte(&(symbol_12x16[2*i][symbol]));
		fbuf[position.page + 1][position.column + i] = pgm_read_byte(&(symbol_12x16[2*i+1][symbol]));
	}
}
//*/