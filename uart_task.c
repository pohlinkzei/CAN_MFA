/*
 * uart_task.c
 *
 * Created: 11.01.2017 22:02:14
 *  Author: Hubert
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "CAN-MFA.h"
#include "uart.h"
#include "dog_display.h"

extern uint8_t cal_ambient_temperature EEMEM;
extern uint8_t cal_voltage EEMEM;
extern uint8_t cal_speed EEMEM;
extern uint8_t cal_oil_temperature EEMEM;
extern uint8_t cal_consumption EEMEM;

volatile struct interrupt_backup{
	uint8_t _TIMSK0;
	uint8_t _TIMSK1;
	uint8_t _TIMSK2;
	uint8_t _TIMSK3;
	//CAN:
	uint8_t _CANGIT;				// clear pending interrupts
	uint8_t _CANSIT1;				// clear interrupt
	uint8_t _CANSIT2;
	//TWI:
	uint8_t _TWCR;
	// ext INT:
	uint8_t _EIMSK;
}interrupt_backup;

void uart_bootloader_init(uint32_t baudrate){
	uart1_init( UART_BAUD_SELECT(baudrate,F_CPU) );
	sei();
	
	uart1_puts_P("\n\rHier ist die CAN MFA...!\n\r");
	uart1_puts("\n\rOptionen: ");
	uart1_puts("\n\rb: Springe zum Bootloader");
	uart1_puts("\n\rc: Calibrierung");
}

void reenable_interrupts(void){
	TIMSK0 = interrupt_backup._TIMSK0;
	TIMSK1 = interrupt_backup._TIMSK1;
	TIMSK2 = interrupt_backup._TIMSK2;
	TIMSK3 = interrupt_backup._TIMSK3;
	CANGIT = interrupt_backup._CANGIT;
	CANSIT1 = interrupt_backup._CANSIT1;
	CANSIT2 = interrupt_backup._CANSIT2;
	EIMSK = interrupt_backup._EIMSK;
	TWCR = interrupt_backup._TWCR;
}

void disable_interrupts(void){
	
	interrupt_backup._TIMSK0 = TIMSK0;
	interrupt_backup._TIMSK1 = TIMSK1;
	interrupt_backup._TIMSK2 = TIMSK2;
	interrupt_backup._TIMSK3 = TIMSK3;
	interrupt_backup._CANGIT = CANGIT;
	interrupt_backup._CANSIT1 = CANSIT1;
	interrupt_backup._CANSIT2 = CANSIT2;
	interrupt_backup._EIMSK = EIMSK;
	interrupt_backup._TWCR = TWCR;
	//timer:
	TIMSK0 = 0x00;
	TIMSK1 = 0x00;
	TIMSK2 = 0x00;
	TIMSK3 = 0x00;
	//CAN:
	CANGIT = 0x00;				// clear pending interrupts
	CANSIT1 = 0x00;				// clear interrupt
	CANSIT2 = 0x00;
	//TWI:
	TWCR = 0x00;
	// ext INT:
	EIMSK = 0x00;
}
//*
int uart_get_int(void){
	char val[4] = {0,};
	char exit_now = 0;
	uint8_t loopcnt = 0;
	int c = ' ';
	do{
		c = uart1_getc();
		if(!(c & UART_NO_DATA))
		{
			if((unsigned char) c >= '0' && (unsigned char) c <= '9'){
				val[loopcnt++] = (unsigned char) c;
				uart1_putc((unsigned char) c);
			}else if(c == 13){
				exit_now = 1;	
				uart1_putc('\n');
				uart1_putc('\r');
			}
		}
	}while(!exit_now);
	return atoi((const char*) val);
}
//*/

void uart_print_cal_menu(void){
	uart1_puts("\n\rCalibrierung: Waehle die Variable ");
	uart1_puts("\n\rv\tcal_voltage ");
	uart1_puts("\n\rs\tcal_speed ");
	uart1_puts("\n\ra\tcal_ambient_temperature");
	uart1_puts("\n\ri\tcal_in_temperature");
	uart1_puts("\n\rg\tcal_gearbox_temperature");
	uart1_puts("\n\ro\tcal_oil_temperature ");
	uart1_puts("\n\rc\tcal_consumption ");
	uart1_puts("\n\rb\tcal_k58b ");
	uart1_puts("\n\rd\tcal_k15_delay ");
	uart1_puts("\n\rm\tcal_can_mode ");
	uart1_puts("\n\re\tEnde ");
}

void uart_calibrate(void){
	
	void (*reset)( void ) = 0x0000;
	
	unsigned int 	c;
	uart_print_cal_menu();

	do{
		c = uart1_getc();
		if(!(c & UART_NO_DATA))
		{
			switch( (unsigned char)c)
			{
				case 'a':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_ambient_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_ambient_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_ambient_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'm':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_can_mode: (0|1) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_can_mode));
					uart1_puts(val);
					eeprom_write_byte(&cal_can_mode, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'g':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_gearbox_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_gearbox_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_gearbox_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'i':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_in_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_in_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_in_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'v':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_voltage: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_voltage));
					uart1_puts(val);
					eeprom_write_byte(&cal_voltage, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 's':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_speed: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_speed));
					uart1_puts(val);
					eeprom_write_byte(&cal_speed, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'o':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_oil_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_oil_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_oil_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'c':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_consumption: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_consumption));
					uart1_puts(val);
					eeprom_write_byte(&cal_consumption, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'b':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_k58b_on_val: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_k58b_on_val));
					uart1_puts(val);
					eeprom_write_byte(&cal_k58b_on_val, uart_get_int());
					uart1_puts("\n\rWert cal_k58b_off_val: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_k58b_off_val));
					uart1_puts(val);
					eeprom_write_byte(&cal_k58b_off_val, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'd':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_k15_delay: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_k15_delay));
					uart1_puts(val);
					
					eeprom_write_byte(&cal_k15_delay, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				//*
				case 'e':{
					//avr_init();
					//return;
					reset();
					//reenable_interrupts();
					//return;
					break;
				}
				//*/
				default:{ 
					/*
					uart1_puts("\n\rCalibrierung: Waehle die variable ");
					uart1_puts("\n\rw\tcal_ambient_temperature");
					uart1_puts("\n\rv\tcal_voltage ");
					uart1_puts("\n\rs\tcal_speed ");
					uart1_puts("\n\ro\tcal_oil_temperature ");
					uart1_puts("\n\rc\tcal_consumption ");
					uart1_puts("\n\rb\tcal_cal_k58b ");
					uart1_puts("\n\re\tEnde ");

					s
					char val[10] = {0,};
					uart1_puts("\n\rDu hast folgendes Zeichen gesendet: ");
					sprintf(val, "0x%X", (unsigned char) c);
					uart1_puts(val);
					uart1_putc(' ');
					uart1_putc((unsigned char)c);
					//*/
					break;
				}
			}
		}
		_delay_ms(200);
		
	}while( (unsigned char) c != 13);
	return;
	//main();
	reset();
}

void uart_bootloader_task(void){
	unsigned int 	c;
	void (*bootloader)( void ) = 0xF800;  // Achtung Falle: Hier Word-Adresse
	
	c = uart1_getc();
	if(!(c & UART_NO_DATA))
	{
		switch( (unsigned char)c)
		{
			case 'b':{
				disable_interrupts();
				dog_clear_lcd();					//	 0123456789012345
				dog_write_mid_string(NEW_POSITION(4,4), "  *BOOTLOADER*  ");
				uart1_puts("\n\rSpringe zum Bootloader...\n\r");
				TCCR3B = 0x00;
				TCCR3A = 0x00;
				
				LED_PORT |= (1<<LED);
				LED_DDR |= (1<<LED);
				
				_delay_ms(1000);
				bootloader();
				break;
			}
			case 'c':{
				disable_interrupts();
				dog_clear_lcd();					//	 0123456789012345
				dog_write_mid_string(NEW_POSITION(4,0), "  *CALIBRATION* ");
				uart1_puts("\n\rCalibrierung...");
				uart_calibrate();
				break;
			}
			default:{
			//char val[10] = {0,};
			uart1_puts("\n\rCAN-MFA USB tool Optionen: ");
			uart1_puts("\n\rb: Springe zum Bootloader");
			uart1_puts("\n\rc: Calibrierung");
			/*sprintf(val, "0x%X", val);
			uart1_puts(val);
			uart1_putc(' ');
			uart1_putc((unsigned char)c);
			//*/
			}
		}
	}
}
