

#include <avr/io.h>           			// Zugriff auf IO's des Controllers
#include <avr/interrupt.h>				// Zugriff auf Interrupts
#include <util/delay.h>					// Funktionalität für Verzögerungen einbinden
#include <inttypes.h>					// Datentyp Integer einbinden
#include <stdbool.h>					// Datentyp BOOL einbinden
#include "can.h"

//////////////////////////////
/****************************/
/*							*/
/*	Untere Anwendungsebene	*/
/*							*/
/****************************/
//////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setBaudrate()											//
//																				//
// Parameter:		uint16_t baud	- zu wählende Baudrate in Kbps				//
//										Es sind die Baudraten 1000, 500, 250	//
//										200, 125 und 100 erlaubt.				//
//																				//
// Rückgabe:		1				- Baudrate erfolgreich eingestellt			//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion setzt die Register für die Baudrate			//
//					entsprechend dem Übergabeparameter 'baud' und der			//
//					in den Projekteinstellungen verwendeten Taktfrequenz.		//
//					Es sind nur die Frequenzen 16, 12, 8, 6 und 4 MHz erlaubt	//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setBaudrate(uint16_t baud){

	#if F_CPU	== 16000000
		switch(baud){
			case 1000:
				CANBT1		= 0x00;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 500:
				CANBT1		= 0x02;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 250:
				CANBT1		= 0x06;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 200:
				CANBT1		= 0x08;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 125:
				CANBT1		= 0x0e;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 100:
				CANBT1		= 0x12;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			default:
				return 0;
				break;
		}
	#elif F_CPU	== 12000000
		switch(baud){
			case 1000:
				CANBT1		= 0x00;
				CANBT2		= 0x08;
				CANBT3		= 0x25;
				return 1;
				break;

			case 500:
				CANBT1		= 0x02;
				CANBT2		= 0x08;
				CANBT3		= 0x25;
				return 1;
				break;

			case 250:
				CANBT1		= 0x04;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 200:
				CANBT1		= 0x04;
				CANBT2		= 0x0e;
				CANBT3		= 0x4b;
				return 1;
				break;

			case 125:
				CANBT1		= 0x0a;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 100:
				CANBT1		= 0x0a;
				CANBT2		= 0x0e;
				CANBT3		= 0x4b;
				return 1;
				break;

			default:
				return 0;
				break;
		}
	#elif F_CPU	==  8000000
		switch(baud){
			case 1000:
				CANBT1		= 0x00;
				CANBT2		= 0x04;
				CANBT3		= 0x13;
				return 1;
				break;

			case 500:
				CANBT1		= 0x00;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 250:
				CANBT1		= 0x02;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 200:
				CANBT1		= 0x02;
				CANBT2		= 0x0e;
				CANBT3		= 0x4b;
				return 1;
				break;

			case 125:
				CANBT1		= 0x06;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 100:
				CANBT1		= 0x08;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			default:
				return 0;
				break;
		}
	#elif F_CPU	==  6000000
		switch(baud){
			case 1000:
				return 0;
				break;

			case 500:
				CANBT1		= 0x00;
				CANBT2		= 0x08;
				CANBT3		= 0x25;
				return 1;
				break;

			case 250:
				CANBT1		= 0x02;
				CANBT2		= 0x08;
				CANBT3		= 0x25;
				return 1;
				break;

			case 200:
				CANBT1		= 0x02;
				CANBT2		= 0x0c;
				CANBT3		= 0x35;
				return 1;
				break;

			case 125:
				CANBT1		= 0x04;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 100:
				CANBT1		= 0x04;
				CANBT2		= 0x0e;
				CANBT3		= 0x4b;
				return 1;
				break;

			default:
				return 0;
				break;
		}
	#elif F_CPU	==  4000000
		switch(baud){
			case 1000:
				return 0;
				break;

			case 500:
				CANBT1		= 0x00;
				CANBT2		= 0x04;
				CANBT3		= 0x13;
				return 1;
				break;

			case 250:
				CANBT1		= 0x02;
				CANBT2		= 0x04;
				CANBT3		= 0x13;
				return 1;
				break;

			case 200:
				CANBT1		= 0x00;
				CANBT2		= 0x0e;
				CANBT3		= 0x4b;
				return 1;
				break;

			case 125:
				CANBT1		= 0x02;
				CANBT2		= 0x0c;
				CANBT3		= 0x37;
				return 1;
				break;

			case 100:
				CANBT1		= 0x02;
				CANBT2		= 0x0e;
				CANBT3		= 0x4b;
				return 1;
				break;

			default:
				return 0;
				break;
		}
	#else
		#warning F_CPU has no correct frequency for baudrate settings
		return 0;
	#endif
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setInterrupt()											//
//																				//
// Parameter:		uint8_t mode	- Ereignis, bei dem ein Interrupt ausgelöst	//
//										werden soll.							//
//										- NONE				- Deaktiviert		//
//										- TX				- Daten gesendet	//
//										- RX				- Daten empfangen	//
//										- TXRX				- Daten gesendet	//
//															und/oder empfangen	//
//																				//
// Rückgabe:		1				- Modus erfolgreich eingestellt				//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion setzt die Interrupts für das Erfolgreiche	//
//					Senden und Empfangen										//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setInterrupt(uint8_t mode){

	switch(mode){
		case NONE:
			clearbit(CANGIE, ENIT);
		  	clearbit(CANGIE, ENRX);
			clearbit(CANGIE, ENTX);
			return 1;
			break;

		case TX:
			setbit(CANGIE, ENIT);
		  	clearbit(CANGIE, ENRX);
			setbit(CANGIE, ENTX);
			return 1;
			break;

		case RX:
			setbit(CANGIE, ENIT);
		  	setbit(CANGIE, ENRX);
			clearbit(CANGIE, ENTX);
			return 1;
			break;

		case TXRX:
			setbit(CANGIE, ENIT);
		  	setbit(CANGIE, ENRX);
			setbit(CANGIE, ENTX);
			return 1;
			break;

		default:
			return 0;
			break;
	}
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_getMOB()												//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
// Rückgabe:		1				- Objekt erfolgreich gewählt				//
//																				//
// Beschreibung: 	Diese Funktion wählt die CANPAGE des betreffenden Objekts	//
//					aus und stellt so einen Zugang zu den Registern des			//
//					Objekts her.												//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_getMOB(uint8_t mob){
	CANPAGE	= (mob << 4);

	return 1;
}


	
//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setIDMask()												//
//																				//
// Parameter:		uint32_t idm	- ID-Maske in Dezimalschreibweise			//
//																				//
// Rückgabe:		1				- Maske erfolgreich eingestellt				//
//																				//
// Beschreibung: 	Diese Funktion setzt die ID-Maske eines Objekts auf einen	//
//					neuen Wert. In CANIDM4 bleiben dabei die Werte der unteren	//
//					3 Bit (RTRTAG, Reserved und IDEMSK) erhalten.				//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setIDMask(uint32_t idm){
	idm		<<= 3;
	idm		|= 7;

	CANIDM4 = (unsigned char) (idm);
	CANIDM3 = (unsigned char) (idm>>8);
	CANIDM2 = (unsigned char) (idm>>16);
	CANIDM1 = (unsigned char) (idm>>24);

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setID()													//
//																				//
// Parameter:		uint32_t id		- ID in Dezimalschreibweise					//
//																				//
// Rückgabe:		1				- ID erfolgreich eingestellt				//
//																				//
// Beschreibung: 	Diese Funktion setzt die ID eines Objekts auf einen			//
//					neuen Wert. In CANIDM4 bleiben dabei die Werte der unteren	//
//					3 Bit (RTRTAG, RB1TAG und RB0TAG) erhalten.					//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setID(uint32_t id){
	id		<<= 3;
	id		&= 0xfffffff8;
	id		|= (CANIDT4 & 0x07);

	CANIDT4 = (unsigned char) (id);
	CANIDT3 = (unsigned char) (id>>8);
	CANIDT2 = (unsigned char) (id>>16);
	CANIDT1 = (unsigned char) (id>>24);

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_getID()													//
//																				//
// Parameter:																	//
//																				//
// Rückgabe:		uint32_t				- ID der empfangenen Nachricht		//
//																				//
// Beschreibung: 	Diese Funktion holt die ID der empfangenen Nachricht.		//
//																				//
//////////////////////////////////////////////////////////////////////////////////
uint32_t CAN_getID(){
	uint32_t	id;

	id = 0;
	id |= ((uint32_t) CANIDT1<<24);
	id |= ((uint32_t) CANIDT2<<16);
	id |= ((uint32_t) CANIDT3<<8);
	id |= (CANIDT4&0xF8);
	id >>= 3;

	return id;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setMode()												//
//																				//
// Parameter:		uint8_t mode	- Betriebsart des Message Objekts:			//
//										- DISABLED			- Deaktiviert		//
//										- TRANSMIT_DATA		- Daten senden		//
//										- TRANSMIT_REMOTE	- Anfrage senden	//
//										- RECEIVE_DATA		- Empfangsmodus		//
//										- AUTO_REPLY		- automatischer		//
//																Antwortmodus	//
//																				//
// Rückgabe:		1				- Modus erfolgreich eingestellt				//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion setzt die Betriebsart des vorher gewählten	//
//					Objekts. Als übergabe dient der gewünschte Modus.			//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setMode(uint8_t mode){

//	setbit(CANCDMOB, IDE);

	switch(mode){
		case DISABLED:
			clearbit(CANCDMOB, CONMOB0);
			clearbit(CANCDMOB, CONMOB1);
			clearbit(CANCDMOB,RPLV);
			clearbit(CANIDT4, RTRTAG);
			clearbit(CANIDM4, RTRMSK);
			return 1;
			break;

		case TRANSMIT_DATA:
			setbit(CANCDMOB, CONMOB0);
			clearbit(CANCDMOB, CONMOB1);
			clearbit(CANCDMOB,RPLV);
			clearbit(CANIDT4, RTRTAG);
			return 1;
			break;

		case TRANSMIT_REMOTE:
			clearbit(CANCDMOB, CONMOB1);
			setbit(CANCDMOB, CONMOB0);
			clearbit(CANCDMOB,RPLV);
			setbit(CANIDT4, RTRTAG);
			return 1;
			break;

		case RECEIVE_DATA:
			clearbit(CANCDMOB, CONMOB0);
			setbit(CANCDMOB, CONMOB1);
			clearbit(CANCDMOB,RPLV);
			clearbit(CANIDT4, RTRTAG);
			return 1;
			break;

		case AUTO_REPLY:
			clearbit(CANCDMOB, CONMOB0);
			setbit(CANCDMOB, CONMOB1);
			setbit(CANCDMOB,RPLV);
			setbit(CANIDT4, RTRTAG);
			return 1;
			break;

		default:
			return 0;
			break;
	}
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_getMode()												//
//																				//
// Parameter:																	//
//																				//
// Rückgabe:		uint8_t mode	- Betriebsart des aktuellen Objekts			//
//																				//
	// Beschreibung: 	Diese Funktion holt die Betriebsart des vorher gewählten	//
	//					Objekts. 													//
//																				//
//////////////////////////////////////////////////////////////////////////////////
uint8_t CAN_getMode(){
	uint8_t		mode;
	mode		= 0;

	if(	!getbit(CANCDMOB, CONMOB1) && !getbit(CANCDMOB, CONMOB0)){
		mode	= DISABLED;
	}else if(	!getbit(CANCDMOB, CONMOB1) && getbit(CANCDMOB, CONMOB0) &&
	!getbit(CANIDT4, RTRTAG)){
		mode	= TRANSMIT_DATA;
	}else if(	!getbit(CANCDMOB, CONMOB1) && getbit(CANCDMOB, CONMOB0) &&
	getbit(CANIDT4, RTRTAG)){
		mode	= TRANSMIT_REMOTE;
	}else if(	getbit(CANCDMOB, CONMOB1) && !getbit(CANCDMOB, CONMOB0) &&
	!getbit(CANIDT4, RTRTAG)){
		mode	= RECEIVE_DATA;
	}else if(	getbit(CANCDMOB, CONMOB1) && !getbit(CANCDMOB, CONMOB0) &&
				getbit(CANCDMOB,RPLV) && getbit(CANIDT4, RTRTAG)){
		mode	= AUTO_REPLY;
	}

	return mode;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setData()												//
//																				//
// Parameter:		uint8_t * data	- Daten-Array über 8 Bytes					//
//																				//
// Rückgabe:		1				- Daten erfolgreich gesetzt					//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion schreibt in das Objekt die zu übermittelnden	//
//					Daten.														//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setData(uint8_t * data){
	uint8_t		i;

	// Anzahl der Datenbytes in der Nachricht
	clearbit(CANCDMOB, DLC0);
	clearbit(CANCDMOB, DLC1);
	clearbit(CANCDMOB, DLC2);
	setbit(CANCDMOB, DLC3);

	for(i = 0; i < 8; i++){
		CANMSG	= data[i];
	}

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_getData()												//
//																				//
// Parameter:																	//
//																				//
// Rückgabe:		uint8_t * data	- Zeiger auf (Empfangs-)daten,				//
//										die im entsprechenden Objekt stehen		//
//																				//
// Beschreibung: 	Diese Funktion liest die in einem Objekt stehenden 			//
//					Nutzdaten aus.												//
//																				//
//////////////////////////////////////////////////////////////////////////////////
CAN_message CAN_getData(){
	uint8_t		i;
	CAN_message	msg;

	for(i = 0; i < 8; i++){
		msg.data[i]	= CANMSG;
	}

	return msg;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_setMOBInterrupt()										//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
// Rückgabe:		1				- Interrupt erfolgreich gesetzt				//
//																				//
// Beschreibung: 	Diese Funktion setzt den Interrupt für das jeweilige Objekt	//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_setMOBInterrupt(uint8_t object){
	switch(object){
		case 0:
			setbit(CANIE2, IEMOB0);
			return 1;
			break;

		case 1:
			setbit(CANIE2, IEMOB1);
			return 1;
			break;

		case 2:
			setbit(CANIE2, IEMOB2);
			return 1;
			break;

		case 3:
			setbit(CANIE2, IEMOB3);
			return 1;
			break;

		case 4:
			setbit(CANIE2, IEMOB4);
			return 1;
			break;

		case 5:
			setbit(CANIE2, IEMOB5);
			return 1;
			break;

		case 6:
			setbit(CANIE2, IEMOB6);
			return 1;
			break;

		case 7:
			setbit(CANIE2, IEMOB7);
			return 1;
			break;

		case 8:
			setbit(CANIE1, IEMOB8);
			return 1;
			break;

		case 9:
			setbit(CANIE1, IEMOB9);
			return 1;
			break;

		case 10:
			setbit(CANIE1, IEMOB10);
			return 1;
			break;

		case 11:
			setbit(CANIE1, IEMOB11);
			return 1;
			break;

		case 12:
			setbit(CANIE1, IEMOB12);
			return 1;
			break;

		case 13:
			setbit(CANIE1, IEMOB13);
			return 1;
			break;

		case 14:
			setbit(CANIE1, IEMOB14);
			return 1;
			break;

		default:
			return 0;
			break;
	}
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_getMOBInterrupt()										//
//																				//
// Parameter:																	//
//																				//
// Rückgabe:		uint8_t mob		- Nummer des Objekts das den Interrupt		//
//										ausgelöst hat							//
//																				//
// Beschreibung: 	Diese Funktion ermittelt, welches Objekt einen Interrupt	//
//					ausgelöst hat.												//
//																				//
//////////////////////////////////////////////////////////////////////////////////
uint8_t CAN_getMOBInterrupt(){
	
	uint8_t 	mob;
	uint16_t	maske;
	maske		= CANSIT2 | (CANSIT1 << 8);

	// Wenn alle 32 Bit der Bitmaske 0 sind dann ist ein Fehler aufgetreten
	if(maske == 0){
		return NOMOB;
	}

	// Die Bitmaske wird so lange nach rechts geschoben, bis Bit0 eine 1 hat.
	// Die Anzahl der Schiebeoperatoren gibt somit die Nummer
	// Des MOBs zurück
	for( mob=0; (maske & 0x01)==0; maske >>= 1, ++mob);

	// Kontrolle: Wenn mob größer als die Anzahl der verfügbaren
	// Message Objects ist das Ergebnis falsch
	if ( mob > 14 ){
		return NOMOB;
	}else{
		return mob;
	}
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_clearMOBInterrupt()										//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
// Rückgabe:		1				- Interrupt erfolgreich gelöscht			//
//																				//
// Beschreibung: 	Diese Funktion löscht den Interrupt für das jeweilige Objekt//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_clearMOBInterrupt(uint8_t object){
	switch(object){
		case 0:
			clearbit(CANIE2, IEMOB0);
			return 1;
			break;

		case 1:
			clearbit(CANIE2, IEMOB1);
			return 1;
			break;

		case 2:
			clearbit(CANIE2, IEMOB2);
			return 1;
			break;

		case 3:
			clearbit(CANIE2, IEMOB3);
			return 1;
			break;

		case 4:
			clearbit(CANIE2, IEMOB4);
			return 1;
			break;

		case 5:
			clearbit(CANIE2, IEMOB5);
			return 1;
			break;

		case 6:
			clearbit(CANIE2, IEMOB6);
			return 1;
			break;

		case 7:
			clearbit(CANIE2, IEMOB7);
			return 1;
			break;

		case 8:
			clearbit(CANIE1, IEMOB8);
			return 1;
			break;

		case 9:
			clearbit(CANIE1, IEMOB9);
			return 1;
			break;

		case 10:
			clearbit(CANIE1, IEMOB10);
			return 1;
			break;

		case 11:
			clearbit(CANIE1, IEMOB11);
			return 1;
			break;

		case 12:
			clearbit(CANIE1, IEMOB12);
			return 1;
			break;

		case 13:
			clearbit(CANIE1, IEMOB13);
			return 1;
			break;

		case 14:
			clearbit(CANIE1, IEMOB14);
			return 1;
			break;

		default:
			return 0;
			break;
	}
}





//////////////////////////////
/****************************/
/*							*/
/*	Obere Anwendungsebene	*/
/*							*/
/****************************/
//////////////////////////////



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_init()													//
//																				//
// Parameter:		uint16_t baud	- zu wählende Baudrate in Kbps				//
//										Es sind die Baudraten 1000, 500, 250	//
//										200, 125 und 100 erlaubt.				//
//																				//
// 					uint8_t intmode	- Ereignis, bei dem ein Interrupt ausgelöst	//
//										werden soll.							//
//										- NONE				- Deaktiviert		//
//										- TX				- Daten gesendet	//
//										- RX				- Daten empfangen	//
//										- TXRX				- Daten gesendet	//
//															und/oder empfangen	//
//																				//
// Rückgabe:		1				- CAN-Controller erfolgreich initialisiert	//
//																				//
// Beschreibung: 	Diese Funktion setzt alle Message Objects zurück und		//
//					Aktiviert den CAN-Controller.								//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_init(uint16_t baud, uint8_t intmode){
	uint8_t		mob;

	/////////////////////////////////////////////////////////////////////
	// Status- und Steuerregister aller Message Objects initialisieren //
	/////////////////////////////////////////////////////////////////////
  	for (mob = 0; mob < NO_MOBS; mob++){
		CANPAGE  = (mob << 4);
		CANSTMOB = 0;
 		CANCDMOB = 0;
	}


	/////////////////////
	// Baudrate setzen //
	/////////////////////
	CAN_setBaudrate(baud);


	///////////////////////////
	// Interrupts aktivieren //
	///////////////////////////
	CAN_setInterrupt(intmode);


  	///////////////////////////////////////////////////
	// Allgemeines CAN Steuerregister initialisieren //
	///////////////////////////////////////////////////
	setbit(CANGCON, ENASTB);		// CAN-Controller in Enabled Mode setzen
  	// Warten bis der CAN-Controller das Enabled-Bit gesetzt hat und einsatzbereit ist
  	while (!getbit(CANGSTA, ENFG));

	return 1; 
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_enableMOB()												//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
//					uint8_t mode	- Betriebsart des Message Objekts:			//
//										- DISABLED			- Deaktiviert		//
//										- TRANSMIT_DATA		- Daten senden		//
//										- TRANSMIT_REMOTE	- Anfrage senden	//
//										- RECEIVE_DATA		- Empfangsmodus		//
//										- AUTO_REPLY		- automatischer		//
//																Antwortmodus	//
//																				//
//					CAN_message		- Struktur mit IDMaske, ID und Nutzdaten	//
//																				//
// Rückgabe:		1				- MOB erfolgreich aktiviert					//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion setzt die Betriebsart des vorher gewählten	//
//					Objekts. Als Übergabe dient der gewünschte Modus.			//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_enableMOB(uint8_t object, uint8_t mode, CAN_message msg){
	
	// Objekt wählen
	CAN_getMOB(object);

	// Interrupt für dieses Objekt aktivieren
	CAN_setMOBInterrupt(object);

	// ID-Maske setzen
	CAN_setIDMask(msg.idm);

	// ID setzen
	CAN_setID(msg.id);

	switch(mode){
		case AUTO_REPLY:
			CAN_setData(msg.data);
			break;
		default:
			break;
	}

	// Betriebsmodus setzen
	CAN_setMode(mode);

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_disableMOB()											//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
// Rückgabe:		1				- MOB erfolgreich deaktiviert				//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion deaktiviert das gewählte Objekt.				//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_disableMOB(uint8_t object){
	
	// Objekt wählen
	CAN_getMOB(object);

	// Interrupt für dieses Objekt aktivieren
	CAN_clearMOBInterrupt(object);

	// Betriebsmodus setzen
	CAN_setMode(DISABLED);

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_sendData()												//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
//					uint8_t * data	- Daten-Array über 8 Bytes					//
//																				//
// Rückgabe:		1				- Daten erfolgreich gesendet				//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion sendet Daten.								//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_sendData(uint8_t mob, uint8_t * data){
	uint8_t		mode;

	// Objekt wählen
	CAN_getMOB(mob);
	
	// Aktuelle Betriebsart sichern
	mode	= CAN_getMode();

	// Nutzdaten in Register schreiben
	CAN_setData(data);

	// Datenübertragung starten
	CAN_setMode(TRANSMIT_DATA);

	// Warten bis die Datenübertragung beendet ist (TXOK-Flag von CAN-Controller gesetzt)
	while (!getbit(CANSTMOB, TXOK));			

	// TXOK-Flag von Hand löschen
	clearbit(CANSTMOB, TXOK);

	// Alte Betriebsart wiederherstellen
	CAN_setMode(mode);

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		CAN_sendRemote()											//
//																				//
// Parameter:		uint8_t mob		- Nummer des zu wählenden Objekts (0-14)	//
//																				//
// Rückgabe:		1				- Anfrage erfolgreich gesendet				//
//					0				- Fehler									//
//																				//
// Beschreibung: 	Diese Funktion sendet eine Anfrage (Remote Frame).			//
//																				//
//////////////////////////////////////////////////////////////////////////////////
int CAN_sendRemote(uint8_t mob){
	uint8_t		mode;

	// Objekt wählen
	CAN_getMOB(mob);
	
	// Aktuelle Betriebsart sichern
	mode	= CAN_getMode();

	// Datenübertragung starten
	CAN_setMode(TRANSMIT_REMOTE);

	// Warten bis die Datenübertragung beendet ist (TXOK-Flag von CAN-Controller gesetzt)
	while (!getbit(CANSTMOB, TXOK));			

	// TXOK-Flag von Hand löschen
	clearbit(CANSTMOB, TXOK);

	// Alte Betriebsart wiederherstellen
	CAN_setMode(mode);

	return 1;
}



//////////////////////////////////////////////////////////////////////////////////
//																				//
// Funktion:		SIGNAL()													//
// 																				//
// Beschreibung: 	Interrupt für den Empfang einer Nachricht.					//
//																				//
//////////////////////////////////////////////////////////////////////////////////
SIGNAL(SIG_CAN_INTERRUPT1){
	uint8_t		save_canpage;
 	static		CAN_message message;

	// Aktuelle CANPAGE sichern
 	save_canpage	= CANPAGE;

    // Index des MOB ermitteln, der den Interrupt ausgelöst hat
	uint8_t mob 	= CAN_getMOBInterrupt();
    
	// Falls es kein gültiges MOB war abbrechen
	if(mob == NOMOB){
		return;
	}

	// Objekt das den Interrupt ausgelöst hat holen
	CAN_getMOB(mob);
    
	// Daten des MOBs aus CANMSG auslesen
	message			= CAN_getData();
	
	// Id der Nachricht holen
	message.id		= CAN_getID();


	//////////////////////////////////
	// Daten verarbeiten / ausgeben //
	//////////////////////////////////


	save_message(message);
/*		if(getbit(message.data[0], 0)){
			LED0_AN;
		}else{
			LED0_AUS;
		}
		if(getbit(message.data[0], 1)){
			LED1_AN;
		}else{
			LED1_AUS;
		}
		if(getbit(message.data[0], 2)){
			LED2_AN;
		}else{
			LED2_AUS;
		}
		if(getbit(message.data[0], 3)){
			LED3_AN;
		}else{
			LED3_AUS;
		}
*/

	//////////////////////////////////
	// Daten verarbeiten / ausgeben //
	//////////////////////////////////


	// RXOK-Flag löschen
	clearbit(CANSTMOB, RXOK);
    
	// MOB auf Empfang und CAN 2.0B Standard setzen
	CAN_setMode(RECEIVE_DATA);
    
    // CANPAGE wiederherstellen
	CANPAGE		= save_canpage;
}

