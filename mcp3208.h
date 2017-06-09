#ifndef __mcp3208_H_
#define __mcp3208_H_

#define SINGLE    0x01        // set to Single-End A/D input
#define DIFFERENTIAL 0x00        // set to Differential A/D input

#define ADC_CS_PORT	PORTF
#define ADC_CS_DDR DDRF
#define ADC_CS       PF0       // SPI PIN for CS

#define XCK_DDR DDRE
#define XCK PE2

#if 1
#define delayCount   ((F_CPU)/1000000UL)

void mcp3208_spi_delay(unsigned int NOPcount);
#endif
void mcp3208_spi_init(void);
uint8_t mcp3208_spi_write(char cData);
uint16_t mcp3208_spi_read(unsigned char AD_type,unsigned char ADchanel);

#endif