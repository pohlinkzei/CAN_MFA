#include <avr/io.h>
#include "mcp3208.h"
#include <util/delay.h>

#define SPI_PORT PORTE
#define SPI_DDR DDRE
#define SPI_PIN PINE
#define MOSI PE1
#define MISO PE0
#define SCK PE2 

#if 1
void mcp3208_spi_delay(unsigned int NOPcount)
{
	unsigned int n;
	for(n=0;n<=NOPcount;n++)
	{
		asm volatile ("nop" ::);
	}
}
#endif
void mcp3208_spi_init(void)
{

	// MOSI und CLK auf Ausgang setzen
	SPI_DDR |= (1<<MOSI);
	SPI_DDR  |= (1<<SCK);

	// MOSI und CLK auf HIGH setzen
	SPI_PORT |= (1<<MOSI);
	SPI_PORT |= (1<<SCK);

	// MISO auf Eingang setzen
	SPI_DDR &= ~(1<<MISO);
	ADC_CS_DDR |= (1<<ADC_CS);
	ADC_CS_PORT |= (1<<ADC_CS);

}

uint8_t mcp3208_spi_write(char dataout)
{
	uint8_t datain=0;
	//das Byte wird Bitweise nacheinander Gesendet MSB zuerst
	for (uint8_t a=8; a>0; a--){
		datain<<=1;					//Schieben um das Richtige Bit zusetzen
		SPI_PORT &=~(1<<SCK);		// Clock auf LOW

		if (dataout & 0x80){		//Ist Bit a in Byte gesetzt
			SPI_PORT |=(1<<MOSI);	//Set Output High
		}
		else{
			SPI_PORT &=~(1<<MOSI);	//Set Output Low
		}
		//mcp3208_spi_delay(delayCount ); //_delay_us(2);
		if (SPI_PIN & (1<<MISO))	//Lesen des Pegels
		{
			datain |= 1;
		}else{
			asm volatile ("nop"::);
		}
		_delay_us(1);
		SPI_PORT |=(1<<SCK);		// Clock auf High
		_delay_us(1);
		asm volatile ("nop"::);
		asm volatile ("nop"::);
		asm volatile ("nop"::);
		asm volatile ("nop"::);

		dataout<<=1;				//Schiebe um nächstes Bit zusenden
	}
	return datain;
}

unsigned int mcp3208_spi_read(uint8_t type,uint8_t channel)
{
	uint8_t  tempHigh,tempLow,tempType,dummy;
	
	ADC_CS_PORT &= ~(1<<(ADC_CS));     //setbitLow CS  Pin
	//mcp3208_spi_delay(delayCount );
	_delay_us(1);
	tempType = (type & 0x01) << 1 ;
	tempLow = (channel & 0x03) << 6;
	tempHigh = (channel & 0x04) >> 2;
	tempHigh |= (0x04)|(tempType);     // 0x04 --> startBit
	
	dummy = mcp3208_spi_write(tempHigh);        // Write control HighByte return not care
	dummy = dummy;
	tempHigh = mcp3208_spi_write(tempLow);  // Write control LowByte return A/D-MSB data
	tempLow = mcp3208_spi_write(0x00);      // Write Null byte 0x00 return A/D-LSB data
	
	//mcp3208_spi_delay(delayCount );
	ADC_CS_PORT |= (1<<(ADC_CS));        //setbitHigh CS  Pin
	
	return (((tempHigh & 0x0F)<<8)|tempLow);  // return 16bit variable (12bit A/D data)
}