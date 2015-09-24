/** INCLUDES *********************************************************************************************************************/
#include "dog_display.h"
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
/** VARIABLES ********************************************************************************************************************/
volatile uint8_t init132[] = {0x40 ,0xA1 ,0xC0 ,0xA6 ,0xA2 ,0x2F ,0xF8 ,0x00 ,0x23 ,0x81 ,0x1F ,0xAC ,0x00 ,0xAF}; 
							//0x40 ,0xA0 ,0xC8 ,0xA6 ,0xA2 ,0x2F ,0xF8 ,0x00 ,0x23 ,0x81 ,0x1F ,0xAC ,0x00 ,0xAF}; 
volatile uint8_t init128[] = {0x40 ,0xA1 ,0xC0 ,0xA6 ,0xA2 ,0x2F ,0xF8 ,0x00 ,0x27 ,0x81 ,0x16 ,0xAC ,0x00 ,0xAF}; 
							//{0x40 ,0xA0 ,0xC8 ,0xA6 ,0xA2 ,0x2F ,0xF8 ,0x00 ,0x27 ,0x81 ,0x16 ,0xAC ,0x00 ,0xAF};
volatile uint16_t k,h,i;
uint8_t reversed = 0;
uint8_t underlined = 0;
uint8_t count;
uint8_t n,m;


/** FUNCTIONS ********************************************************************************************************************/
void dog_spi_init(void){
	SPI_DDR |= (1<<MOSI) | (1<<MISO) | (1<<SCK);
	DOG_A0_DDR |= (1<<DOG_A0);
	DOG_RST_DDR |= (1<<DOG_RST);
	SPCR = 0;
	//SPCR = (1<<SPE) | (1<<MSTR)| (0<<DORD) | (1<<CPOL) | (1<<CPHA);// | (1<<SPR0)  | (1<<SPR1);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1) | (0<<DORD) | (1<<CPOL) | (1<<CPHA);
	//SPSR = (1<<SPI2X);

	uint8_t dummy = (SPSR);
	dummy=dummy;
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
uint8_t dog_transmit(uint8_t data){
	SPDR = data;
	while(!(SPSR & (1<<SPIF))){;}
	//_delay_us(50);
	return SPDR;
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_transmit_data(uint8_t data){
	DOG_A0_HIGH();
 	if(reversed){
 		data = 0xFF - data;
 	}
	dog_transmit(data);
	DOG_A0_LOW();
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_set_page(uint8_t page){
	dog_transmit(SET_PAGE + page);
	dog_transmit(SET_ADDRESS0);
	dog_transmit(SET_ADDRESS1);
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
position_t dog_set_position(uint8_t page,uint8_t column){
	position_t position;
	position.page = page;
	position.column = column;
	dog_transmit(SET_PAGE + page);
	dog_transmit(SET_ADDRESS0 + ((column & 0xF0)>>4));
	dog_transmit(SET_ADDRESS1 + (column & 0x0F));
	return position;
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
position_t NEW_POSITION(uint8_t p, uint8_t c){
	position_t pos;
	pos.page = p;
	pos.column = c;
	pos.row = 0;
	return pos;
}	
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_home(void){
	dog_set_page(0);
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
#ifdef USE_SMALL_FONT
void dog_write_small_digit(uint8_t digit){
	uint8_t i,data;
#ifdef CHARSET_ASCII
	//digit -= 0x20;
#endif	
	for(i=0;i<6;i++){
		data = pgm_read_byte(&(font6x8[digit][i]));
		dog_transmit_data(data);
	}
	
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_small_string(char *string){
	while(*string){
		dog_write_small_digit(*string);
		string++;
	}
}
#endif
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_numbered_bat_symbol(position_t position, uint8_t num){
	//if(num>9) return;
	
	uint8_t temp[3][24] = {{0}};
	//uint8_t upper[12];
	//uint8_t lower[12];
	position.row = position.row % 8;
	uint32_t temprow = 0;
	
	
	//char frame[48] = {0};
	for(i=0; i<12; i++){
		//lower
		temp[1][i] = pgm_read_byte(&(font12x16[BAT][2*i]));
		//upper
		temp[0][i] = pgm_read_byte(&(font12x16[BAT][2*i+1]));
		
		//frame[2*i] 		= pgm_read_byte(&(font12x16[BAT][2*i+1]));
		//frame[2*i+1]	= pgm_read_byte(&(font12x16[BAT][2*i]));
	}
	for(i=12; i<24; i++){
		//lower
		temp[1][i] = pgm_read_byte(&(font12x16[BAT][2*i]));
		//upper
		temp[0][i] = pgm_read_byte(&(font12x16[BAT][2*i+1]));
	}
	char number[16] = {0};
	for(i=0; i<8; i++){
		uint16_t temp = pgm_read_byte(&(font8x12[num][2*i+1]));
		temp = temp << 8;
		temp += pgm_read_byte(&(font8x12[num][2*i]));
		temp = temp << 3;
		number[2*i+1]	= (uint8_t) temp;
		number[2*i]		= (uint8_t) (temp>>8);
	}
	for(i=0; i<8; i++){
		
		temp[1][9+i] ^= number[2*i];
		temp[0][9+i] ^= number[2*i+1];
	}
	if(position.row != 0){
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
		dog_set_position(position.page,position.column);
		for(i=0;i<24;i++){
			dog_transmit_data(temp[0][i]);
		}
		dog_set_position(position.page+1,position.column);
		for(i=0;i<24;i++){
			dog_transmit_data(temp[1][i]);
		}
	}
}

/**-----------------------------------------------------------------------------------------------------------------------------**/
#ifdef USE_MID_FONT
void dog_write_mid_digit(position_t position,uint8_t digit){
	uint8_t i;
#ifdef CHARSET_ASCII
	//digit -= 0x20;
#endif
	uint8_t temp[3][8] = {{0,},};
	position.row = position.row % 8;
	uint32_t temprow = 0;
	for(i=0;i<8;i++){
		//lower
		temp[0][i] = pgm_read_byte(&(font8x12[digit][2*i]));
		//upper
		temp[1][i] = pgm_read_byte(&(font8x12[digit][2*i+1]));
	}
	/*
	uint8_t upper[8];
	uint8_t lower[8];
	for(i=0;i<8;i++){
		lower[i] = pgm_read_byte(&(font8x12[digit][2*i+1]));
		if(underlined){
			lower[i] |= 0x30;
		}
		upper[i] = pgm_read_byte(&(font8x12[digit][2*i]));
	}
	*/
	if(position.row == 0){
		dog_set_position(position.page,position.column);
		for(i=0;i<8;i++){
			dog_transmit_data(temp[0][i]);
			//dog_transmit_data(upper[i]);
		}
		dog_set_position(position.page+1,position.column);
		for(i=0;i<8;i++){
			dog_transmit_data(temp[1][i]);
			//dog_transmit_data(lower[i]);
		}	
	}else{
		for(i=0; i<8; i++){
			temprow = 0;
			temprow = ((temp[0][i]) + ((uint16_t) temp[1][i] << 8) + ((uint32_t) temp[2][i] << 16));
			temprow = temprow << position.row;
			temp[0][i] = (uint8_t) ((temprow) & 0x000000FF);
			temp[1][i] = (uint8_t) ((temprow>>8) & 0x000000FF);
			temp[2][i] = (uint8_t) ((temprow>>16) & 0x000000FF);
		}
		
		dog_set_position(position.page, position.column);
		for(i=0; i<8; i++){
			dog_transmit_data(temp[0][i]);
		}
		dog_set_position(position.page+1, position.column);
		for(i=0; i<8; i++){
			dog_transmit_data(temp[1][i]);
		}
		dog_set_position(position.page+2, position.column);
		for(i=0; i<8; i++){
			dog_transmit_data(temp[2][i]);
		}
	}
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_mid_string(position_t position,char *string){
	while(*string){
		dog_write_mid_digit(position,*string);
		string++;
		position.column+= 8;
	}
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_mid_strings(position_t pos, char *str0, char *str1){
	volatile uint8_t line0[132] = {0}, line1[132] = {0}, line2[132] = {0};
	uint8_t h=0, i=0, offset=0;
#ifdef CHARSET_ASCII
	offset = 0x20;
#endif
	for(h=0; h<16;h++){
		//uint8_t digit = str0[h];
		for(i=0;i<8;i++){
			line0[8*h+i] = pgm_read_byte(&(font8x12[(unsigned char) str0[h] - offset][2*i]));
			line1[8*h+i] = pgm_read_byte(&(font8x12[(unsigned char) str0[h] - offset][2*i+1]));
		}
	}
	for(h=0; h<16;h++){
		for(i=0;i<8;i++){
			uint16_t temp = (pgm_read_byte(&(font8x12[(unsigned char) str1[h] - offset][2*i+1]))<<8) 
							+ pgm_read_byte(&(font8x12[(unsigned char) str1[h] - offset][2*i]));
			temp = temp << 4;
			line1[8*h+i] |= (uint8_t) (temp & 0x00FF);
			line2[8*h+i] = (uint8_t) (temp>>8);
		}
	}
	dog_set_position(pos.page,pos.column + 4);
	for(i=0;i<132;i++){
		dog_transmit_data(line0[i]);
	}
	dog_set_position(pos.page +1 ,pos.column + 4);
	for(i=0;i<132;i++){
		dog_transmit_data(line1[i]);
	}
	dog_set_position(pos.page + 2 ,pos.column + 4);

	for(i=0;i<132;i++){
		dog_transmit_data(line2[i]);
	}

}
#endif
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_rotating(position_t position, char* text, uint8_t length, uint8_t type, volatile uint16_t tim){
	// position: Startposition der laufschrift
	// text: der Text, der umläuft höhö
	// length: die Textlänge
	// tim: eine Referenz auf die timervariable (womöglich gibt es mehrere parallele Laufschriften)
	//idee: wenn der Text länger ist, als die Displaybreite hergibt, rotiert der Text im 10s Abstand dreimal um das Display.
	// 
	uint8_t line_length = 0;
	dog_set_position(position.page, position.column);
	line_length = COLUMNS / type;
	uint16_t rot = tim % ((line_length + length)*type);
	if(length < line_length){
		switch(type){
			#ifdef USE_TINY_FONT
			case SIZE4X6:{
				dog_write_tiny_string(text);
				break;
			}
			#endif
			#ifdef USE_SMALL_FONT
			case SIZE6X8:{
				dog_write_small_string(text);
				break;
			}
			#endif
			#ifdef USE_MID_FONT
			case SIZE8X12:{
				dog_write_mid_string(position, text);
				break;
			}	
			#endif
			#ifdef USE_BIG_FONT
			case SIZE12X16:{
				dog_write_big_string(position, text);
				break;
			}
			#endif
			default:
			
			return;
		}
		return;
	}
	char line_text[22] = {0,};
	position.column -= rot % type;
	dog_set_position(position.page, position.column);
	for(i=0; i<line_length;i++){
		int8_t ti = i+(rot/type)-line_length;
		if(ti < 0){
			line_text[i] = ' ';
		}else if(ti>length){
			line_text[i] = ' ';
		}else{	
			line_text[i] = text[ti];
		}		
	}
	switch (type){
		#ifdef USE_TINY_FONT
		case SIZE4X6:{
			dog_write_tiny_string(line_text);
			break;
		}
		#endif
		#ifdef USE_SMALL_FONT
		case SIZE6X8:{
			dog_write_small_string(line_text);
			break;
		}
		#endif
		#ifdef USE_MID_FONT
		case SIZE8X12:{
			dog_write_mid_string(position, line_text);
			break;
		}
		#endif
		#ifdef USE_BIG_FONT
		case SIZE12X16:{
			dog_write_big_string(position, line_text);
			break;
		}
		#endif
		default: 
		
		return;
	}
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_empty_line(position_t p){
	uint8_t i;
	dog_set_position(p.page, p.column);
	for(i=0;i<132-p.column;i++){
		dog_transmit_data(0x00);
	}
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
#ifdef USE_BIG_FONT
void dog_write_big_digit(position_t position,uint8_t digit){
	uint8_t i = 0;
#ifdef CHARSET_ASCII
	//digit -= 0x20;
#endif
	uint8_t temp[3][12] = {{0,},};
	position.row = position.row % 8;
	uint32_t temprow = 0;
	for(i=0;i<12;i++){
		//lower
		temp[1][i] = pgm_read_byte(&(font12x16[digit][2*i]));
		//upper
		temp[0][i] = pgm_read_byte(&(font12x16[digit][2*i+1]));
	}
	if(position.row != 0){
		for(i=0; i<12; i++){
			temprow = 0;
			temprow = ((temp[0][i]) + ((uint16_t) temp[1][i] << 8) + ((uint32_t) temp[2][i] << 16));
			temprow = temprow << position.row;
			temp[0][i] = (uint8_t) ((temprow) & 0x000000FF);
			temp[1][i] = (uint8_t) ((temprow>>8) & 0x000000FF);
			temp[2][i] = (uint8_t) ((temprow>>16) & 0x000000FF);	
		}	
		
		dog_set_position(position.page, position.column);
		for(i=0; i<12; i++){
			dog_transmit_data(temp[0][i]);
		}
		dog_set_position(position.page+1, position.column);
		for(i=0; i<12; i++){
			dog_transmit_data(temp[1][i]);
		}
		dog_set_position(position.page+2, position.column);
		for(i=0; i<12; i++){
			dog_transmit_data(temp[2][i]);
		}
		
		
	}else{
		dog_set_position(position.page,position.column);
		for(i=0;i<12;i++){
		
			dog_transmit_data(temp[0][i]);
		}
		dog_set_position(position.page+1,position.column);
		for(i=0;i<12;i++){
		
			dog_transmit_data(temp[1][i]);
		}
	}		
		
	//*/
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_big_string(position_t position,char *string){
	while(*string){
		dog_write_big_digit(position,*string);
		string++;
		position.column+= 12;
	}
}
#endif
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_clear_lcd(void){
	uint8_t i,h,clear;
	dog_home();
 	if(reversed){
 		clear = 0xFF;
 	}else{
		clear = 0x00;
	}
	for(h=0;h<ROWS;h++){
		dog_set_page(h);
		for(i=0;i<COLUMNS;i++){
			dog_transmit_data(clear);
		}
	}
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
#ifdef USE_TINY_FONT
void dog_write_tiny_string(char *string){
	while(*string){
		dog_write_tiny_digit(*string);
		string++;
	}
}

/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_write_tiny_digit(uint8_t digit){
	uint8_t i,data;
#ifdef CHARSET_ASCII
	//digit -= 0x20;
#endif
	for(i=0;i<4;i++){
		data = pgm_read_byte(&(font4x6[digit][i]));
		dog_transmit_data(data);
	}

}
#endif
/**-----------------------------------------------------------------------------------------------------------------------------**/
position_t NEW__POSITION(uint8_t page,uint8_t column, uint8_t row){
	position_t pos = NEW_POSITION(page,column);
	pos.row = row;
	return pos;
}
/**-----------------------------------------------------------------------------------------------------------------------------**/
void dog_init(void){
	uint8_t i = 0;
	DOG_RST_HIGH();
	_delay_us(100);
	DOG_RST_LOW();
	_delay_us(100);
	DOG_RST_HIGH();
	_delay_ms(1);
	for(i=0;i<14;i++){
		dog_transmit(init128[i]);
	}
}
