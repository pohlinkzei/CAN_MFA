#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/wdt.h>
#include "kline.h"
#include "uart.h"
#include "CAN-MFA.h"

#define SYNC 0x55
#define KEY_WRD_LOW 0x01
#define KEY_WRD_HIGH 0x8A

#define NO_ERROR 0xFFFF88

#define CLR_ERR 0x05 //- clear errors
#define COMM_END 0x06 //- end output
#define GET_ERR 0x07 //- get errors
#define ACK 0x09 //- ack command
#define READ_GRP 0x29 //- group reading
#define RESP_READ_GRP 0xE7 //- response on group reading
#define ASCII 0xF6 //- ASCII
#define RESP_GET_ERR 0xFC //- response on get errors
#define BLK_END 0x03


void kline_uart_init(uint16_t baudrate){
	uart_init( UART_BAUD_SELECT(baudrate,F_CPU) );
}

void kline_io_init(void){
	KLINE_DDR |= (1<<KLINE_TX);
}



void uart_clear ()
{
 //TODO: reset UART0
}


uint16_t kline_read_byte(void){
	return 0;
}

uint16_t ser_getc (void)		
{
	uint16_t c;
	c = uart_getc();
	if(!(c & UART_NO_DATA)){
		wdt_reset();
		return (c);
	}
	return 0;
}


void uart_disable ()
{
	//TODO: DISABLE UART
}


volatile uint16_t	block_cnt,
					block_length;

void  kline_get_ids ();


uint16_t kline_block_counter()
{
	block_cnt++;
	if (block_cnt > 0xFF)
		block_cnt = 0;
	return block_cnt;
}
 

void kline_init (uint8_t unit_address)
{
	uint16_t ms=200;				// 5Bd = 5 Bits/s => 1000/5 = 200ms/Bit. 

	// Adresse 0x01 (1): ECU
	KLINE_PORT |= (1 << KLINE_TX);
	wdt_reset();
	wdt_reset();
	KLINE_PORT &= ~(1 << KLINE_TX);	// KLINE_TX aus	: Start-Bit 1->0
	_delay_ms (ms);

	if(unit_address & 0x01)
		KLINE_PORT |= (1 << KLINE_TX);
	else 
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);

	if(unit_address & 0x02)
		KLINE_PORT |= (1 << KLINE_TX);
	else
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);
	
	if(unit_address & 0x04)
		KLINE_PORT |= (1 << KLINE_TX);
	else
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);

	if(unit_address & 0x08)
		KLINE_PORT |= (1 << KLINE_TX);
	else
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);

	if(unit_address & 0x10)
		KLINE_PORT |= (1 << KLINE_TX);
	else
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);

	if(unit_address & 0x20)
		KLINE_PORT |= (1 << KLINE_TX);
	else
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);

	if(unit_address & 0x40)
		KLINE_PORT |= (1 << KLINE_TX);
	else
		KLINE_PORT &= ~(1 << KLINE_TX);
	_delay_ms (ms);
		
	wdt_reset();

	KLINE_PORT &= ~(1 << KLINE_TX);	// KLINE_TX low	: Odd Parity
	_delay_ms (ms);									
	KLINE_PORT |= (1 << KLINE_TX);		// KLINE_TX high	: Stop-Bit
	_delay_ms (ms);									

}



int8_t kline_sync ()
{
	uint16_t	msg[3], 
			i;
	
	for (i=0; i<=2; i++){
		msg[i] = ser_getc();
}

	if ((msg[0] == SYNC) && (msg[1] == KEY_WRD_LOW) && (msg[2] == KEY_WRD_HIGH))
	{
		uart_putc (0xFF - msg[2]);
		ser_getc();
		return 1;
	}
	else
		return -1;
}



uint16_t kline_get_byte ()
{
	uint16_t	msg;
	
	msg = ser_getc();
	uart_putc (0xFF-msg);
	ser_getc();
	return msg;
}

void kline_send_ack ()
{
	uart_putc (0x03);	
	ser_getc();				// ignore echo
	ser_getc();				// ignore response

	uart_putc (kline_block_counter());	
	ser_getc();				// ignore echo
	ser_getc();				// ignore response

	uart_putc (ACK);	
	ser_getc();				// ignore echo
	ser_getc();				// ignore response

	uart_putc (BLK_END);	
	ser_getc();				// ignore echo
}


void kline_get_ack ()
{
	uint16_t	msg;
	
	msg = ser_getc();		// Block length
	uart_putc (0xff-msg);	// send response
	ser_getc();				// ignore echo

	msg = ser_getc();		// Block counter
	uart_putc (0xff-msg);	// send response
	ser_getc();				// ignore echo
	block_cnt = msg;

	msg = ser_getc();		// Block title
	uart_putc (0xff-msg);	// send response
	ser_getc();				// ignore echo

	msg = ser_getc();		// ignore Block end 
}


uint16_t kline_get_header (void){
	block_length = kline_get_byte ();
	block_cnt = kline_get_byte ();
	return kline_get_byte ();
}	


void kline_get_ascii (char *_string)
{
	uint16_t	ecu,
			i;
	
	for (i=1; i<=block_length-3; i++)
	{
		ecu = kline_get_byte ();
		
		if ((ecu < 32) || (ecu > 122))
			_string[i-1] = '?';
		else
			_string[i-1] = ecu;
	}
	
	_string[i-1] = '\0';
	
	ser_getc();
	
//	_delay_ms (100);
}


uint8_t kline_get_group_data (uint16_t _values[])
{
	uint16_t	ecu,
				i;
	
	for (i=1; i<=block_length-3; i++)
	{
		ecu = ser_getc();
		
		uart_putc (0xFF-ecu);
		_values[i-1] = ecu;
		ser_getc();				// ignore echo
	}
	ser_getc();
	return 1;
}


void kline_get_group (uint16_t group)
{
	uint16_t	ecu, ecu1;

	uart_putc (0x04);
	ser_getc();
	ecu = ser_getc();

	uart_putc (kline_block_counter());	// Block Counter
	ecu1 = ser_getc();
	ecu = ser_getc();

	uart_putc (READ_GRP);		// Ask for group reading
	ser_getc();
	ser_getc();

	uart_putc (group);		// request Group
	ser_getc();
	ser_getc();

	uart_putc (BLK_END);
	ser_getc();
	ecu1 = ecu;
	ecu = ecu1;
}


uint8_t kline_check_err ()
{
	uint8_t	header, ecu, ecu1, ecu2, i, error = 1;

	uart_putc (0x03);
	ser_getc();
	ser_getc();

	uart_putc (kline_block_counter());
	ser_getc();
	ser_getc();

	uart_putc (GET_ERR);		// Ask for errors
	ser_getc();
	ser_getc();

	uart_putc (BLK_END);
	ser_getc();

	do
	{
		header = kline_get_header ();
		
		if (header == RESP_GET_ERR)
		{
			ecu = ser_getc();		// byte 1
			uart_putc (0xFF-ecu);
			ser_getc();
			
			ecu1 = ser_getc();		// byte 2
			uart_putc (0xFF-ecu1);
			ser_getc();
			
			ecu2 = ser_getc();		// byte 3
			uart_putc (0xFF-ecu2);
			ser_getc();
			
			for (i=1; i<=block_length-6; i++)	// 3+3 Bytes der block length bereits verarbeitet
			{
				ecu = ser_getc();		// weitere fehlerbytes interessieren jetzt nicht
				uart_putc (0xFF-ecu);
				ser_getc();
			}
			ser_getc();				// Block-end Byte ignorieren
			
			if ((ecu == 0xFF) && (ecu1 == 0xFF) && (ecu2 == 0x88))	// kein Fehler gespeichert
				error = 0;
			
			kline_send_ack ();
		}
		else
		{
			ser_getc();
		}
		
	} while (header == 0xFC);
	
	return error;
}

void  kline_wakeup ()
{
	uint16_t 	i, baud_rates[] = {9600, 10400, 4800};
	i = 0;
	do
	{
		wdt_reset();
		_delay_ms (1000);
		uart_disable();
		wdt_reset();
		kline_init(0x01 /* ECU */);
		wdt_reset();
		kline_uart_init(baud_rates[i]);
		i++;
		if (i >= 2)
			i=0;
	} while (kline_sync() < 0);
}


void  kline_get_ids ()
{
	uint16_t	cnt,
				block_title;
	char 		texte[6][40];

	cnt=0;
	do
	{
		block_title = kline_get_header();
		if (block_title == ASCII)
		{
			kline_get_ascii (texte[cnt]);
			cnt++;
			kline_send_ack ();
		}
		else
			ser_getc();
		
	} while (block_title == ASCII);
}


void  kline_display_values (void){
	if (kline_check_err ()){		// Fehler vorhanden?
		//TODO: Indicate error
	}
}

//
//	@brief	Hauptroutine
//
void kline_task(void)
{
/*
	if ((MCUCSR & (1 << EXTRF)) || (MCUCSR & (1 << PORF)) || (MCUCSR & (1 << BORF)))		// external, power-on- oder brown-out-reset
	{

	}
	if ((MCUCSR & (1 << WDRF)))		// watchdog-reset
	{

	}
	*/
	wdt_enable (WDTO_2S);
	kline_wakeup ();
	kline_get_ids ();			// dummy read out
	kline_display_values();
}
