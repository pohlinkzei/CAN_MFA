#ifndef CAN_H
#define CAN_H

/****************/
/* Bibliotheken */
/****************/

#include <avr/io.h>           			// Zugriff auf IO's des Controllers
#include <avr/interrupt.h>				// Zugriff auf Interrupts
#include <util/delay.h>					// Funktionalität für Verzögerungen einbinden
#include <inttypes.h>					// Datentyp Integer einbinden
#include <stdbool.h>					// Datentyp BOOL einbinden



/****************/
/* Definitionen */
/****************/


/* Betriebsmodus */
enum{DISABLED, TRANSMIT_DATA, TRANSMIT_REMOTE, RECEIVE_DATA, AUTO_REPLY};


/* Interrupt-Modi */
enum{NONE, TX, RX, TXRX};


/* CAN Objekte */
#define NO_MOBS 15									// Anzahl der Message Objects festlegen
#define NOMOB   0xff								// Definition für ungültigen Objektindex


/* Datenstruktur für eine CAN-Nachricht der Version 2.0B */
typedef struct{	
	uint32_t	id;								// Identifier (29 Bit)
	uint32_t	idm;							// ID-Maske
	uint8_t		length;							// Nachrichtenlänge
	uint8_t		data[8];						// Daten
}CAN_message;



/**************************/
/* Funktionsdeklarationen */
/**************************/

extern void save_message(CAN_message msg);
/* Obere Andwndungsebene */
int			CAN_init(uint16_t baud, uint8_t intmode);
int 		CAN_enableMOB(uint8_t object, uint8_t mode, CAN_message msg);
int 		CAN_disableMOB(uint8_t object);
int 		CAN_sendData(uint8_t mob, uint8_t * data);
int 		CAN_sendRemote(uint8_t mob);


/* Untere Anwendungsebene */
int 		CAN_setBaudrate(uint16_t baud);
int 		CAN_setInterrupt(uint8_t mode);
int 		CAN_getMOB(uint8_t mob);
int 		CAN_setIDMask(uint32_t idm);
int 		CAN_setID(uint32_t id);
uint32_t 	CAN_getID();
int 		CAN_setMode(uint8_t mode);
uint8_t 	CAN_getMode();
int 		CAN_setData(uint8_t * data);
CAN_message CAN_getData();
int 		CAN_setMOBInterrupt(uint8_t object);
uint8_t 	CAN_getMOBInterrupt();
int 		CAN_clearMOBInterrupt(uint8_t object);



/* Bitebene */
#define 	setbit(ADR,BIT)		(ADR|=(1<<BIT))			// Makro zum Setzen von Bits
#define 	clearbit(ADR,BIT)	(ADR&=~(1<<BIT))		// Makro zum Löschen von Bits
#define 	getbit(ADR, BIT)	(ADR & (1<<BIT))		// Makro zum Abfragen eines Bits

#endif