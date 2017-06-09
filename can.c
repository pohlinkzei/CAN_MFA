#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
//#include <stdbool.h>
#include "can.h"
#include "CAN-MFA.h"

uint8_t temp_buffer[8];
uint8_t temp_data[8];
uint16_t cons_old = 0;
uint64_t can_speed_sum = 0;
uint32_t can_speed_cnt = 0;			
static volatile uint8_t can_irq = 0;
void can_read_data(void);

uint16_t mob_id[16] = {
	0x280, 
	0x288, 
	0x380, 
	0x480, 
	0x320, 
	0x420, 
	0x520, 
	0x5D0, 
	0x5D8, 
	0x7FF,
	0x7FF,
	0x7FF,
	0x7FF,
	0x7FF,
	0x7FF,
	0x7FF,
};

 void can_normal_id (uint16_t id_tag, uint8_t id_flags, uint8_t mask_type )
{
    if ( mask_type) 
	{
		CANIDM1 = (uint16_t) id_tag >> 3 ;
		CANIDM2 = (uint8_t) id_tag << 5 ;
		CANIDM3 = 0 ;
		CANIDM4 = id_flags ;
    } 
	else 
	{
		CANIDT1 = (uint16_t) id_tag >> 3 ;
		CANIDT2 = (uint8_t) id_tag << 5 ;
		CANIDT3 = 0 ;
		CANIDT4 = id_flags ;
    }
}

	    // Extended address range (V2.0 part B)

 void can_extended_id (can_ID id_tag, uint8_t id_flags, uint8_t mask_type )
{
    id_tag.ci_i <<= 3 ;
    if ( mask_type ) {
	CANIDM1 = id_tag.ci_b[3] ;
	CANIDM2 = id_tag.ci_b[2] ;
	CANIDM3 = id_tag.ci_b[1] ;
	CANIDM4 = id_tag.ci_b[0] | id_flags ;
    } else {
	CANIDT1 = id_tag.ci_b[3] ;
	CANIDT2 = id_tag.ci_b[2] ;
	CANIDT3 = id_tag.ci_b[1] ;
	CANIDT4 = id_tag.ci_b[0] | id_flags ;
    }
}

uint8_t get_mob_by_id(uint16_t id){
	uint8_t i;
	for(i=0; i<15; i++){
		if(mob_id[i] == id){
			return i;
		}
	}
	return 16;
}

uint8_t check_mob_ready(uint8_t mob){
	if(mob > 7){
		if(!(CANEN1 & (1<<(mob - 8))))
			return 1;	
	}else{
		if(!(CANEN2 & (1<<mob)))
			return 1;
	}
	return 0;
}

// CAN interrupt handler		--> Interrupt wird zur Zeit nicht verwendet
// ---------------------
//*
ISR ( CANIT_vect )		
{
	can_irq = 1;
}


ISR(OVRIT_vect)
{
	//uart_puts( "overflow" ); 
}
//*/
void can_init( void )
{
    uint8_t		i;

    CANGCON |= ( 1<<SWRES ) ;			// Reset CAN interface

	// Initialise the MOBs

    for ( i = 0; i < 15; i++ ) 	// alle MOBs auf den Empfang der Botschaft 0x7FF (2047) einstellen
	{								// --> dadurch sind die MOBs praktisch "deaktiviert" (setzt voraus, dass
									// 0x7FF nicht auf dem CAN-Bus versendet wird
		CANPAGE = i << 4 ;        	// select MOB
		CANSTMOB = 0 ;				// clear status
	    CANCDMOB = (1<<CONMOB1) ;
	    can_normal_id ((uint16_t) 2047, 0, 0) ;
	    can_normal_id ((uint16_t) 2047, 0, 1) ;
	} 
	for(i = 0; i<15; i++){
		CANPAGE=i << 4;
		CANSTMOB = 0 ;
		CANCDMOB = (1<<CONMOB1) ;
	    can_normal_id ((uint16_t) mob_id[i], 0, 0) ;
	    can_normal_id ((uint16_t) 2047, 0, 1) ;
	}

	// CAN-Baudrate auf 500 kbit/s einstellen
	if(F_CPU == 16000000UL){
		CANBT1 = 0x02 ;
		CANBT2 = 0x0C ;
		CANBT3 = 0x37 ;
	}else if(F_CPU == 8000000UL){
//*		CAN_BAUDRATE == 500       //!< -- 500Kb/s, 8x Tscl, sampling at 75%
		CANBT1 =   0x02  ;     // Tscl  = 2x Tclkio = 250 ns
		CANBT2 =   0x04  ;     // Tsync = 1x Tscl, Tprs = 3x Tscl, Tsjw = 1x Tscl
		CANBT3 =   0x13  ;    // Tpsh1 = 2x Tscl, Tpsh2 = 2x Tscl, 3 sample points
//*/		
	}else if(F_CPU == 2000000UL){
		
	}else{ //F_CPU == 1000000UL
		
	}
	
	    // Clear CAN interrupt registers, Interrupts werden zwar eingestellt, dann aber trotzdem vorerst
		// nicht verwendet
    CANGIT = 0 ;				// clear pending interrupts
    CANSIT1 = 0 ;				// clear interrupt
    CANSIT2 = 0 ;				//     	status registers

	    // Start CAN interface

    CANGCON = (1<<ENASTB) ;
	CANGSTA &= ~( 1<< ENFG);
	CANPAGE |= (1<<AINC);
}

void can_task(void){
	can_read_data();
	if(K15_PIN & (1<<K15)){
		//read data
		if(id280_valid){
			// [status] [ 1 ] [ 2 ] [rpm] [rpm] [ 5 ] [ pedal_position ] [ 7 ]
			// calculate rpm
			rpm = (id280_data[4] + ((id280_data[3]) << 8))>>2;
			eng_status0 = id280_data[0];
			pedal_position = id280_data[6];
			id280_valid = 0;
			can_status |= (1<<ID280);
		}
		if(id288_valid){
			// [ 0 ] [ eng_temp ] [ status1 ] [ spd ] [spd_gra] [ 5 ] [ 6 ] [ 7 ]
			can_speed_cnt++;
			engine_temperature = id288_data[1] - 100;
			if(engine_temperature < 25)
				engine_temperature = 25;
			uint16_t spd = ((id288_data[3]));
			gra_speed = id288_data[4];
			eng_status1 = id288_data[2];
	
			can_speed_sum += spd;
			id288_valid = 0;
			can_status |= (1<<ID288);
		}
		if(id380_valid){
			// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [4] [ 5 ] [ 6 ] [ 7 ]
			id380_valid = 0;
			can_status |= (1<<ID380);
		}
		if(id480_valid){
			// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [4] [ 5 ] [ 6 ] [ 7 ]
			can_status |= (1<<ID480);
			//id480_valid = 0;
		}
		if(id320_valid){
			// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [4] [ 5 ] [ 6 ] [ 7 ]
			can_status |= (1<<ID320);
			id320_valid = 0;
		}
		if(id420_valid){
			// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [4] [ 5 ] [ 6 ] [ 7 ]
			can_status |= (1<<ID420);
			id420_valid = 0;
		}
		if(id520_valid){
			// [ 0 ] [ 1 ] [ 2 ] [ 3 ] [4] [ 5 ] [ 6 ] [ 7 ]
			can_status |= (1<<ID520);
			id520_valid = 0;
		}
	}
}

uint16_t can_get_normal_id(){
	uint16_t id;
	id = CANIDT1 << 3 | ((CANIDT2 >> 5) & (0x07));
	return id;
}


void can_read_data(){
	//if(can_irq){
		uint8_t i, dlc;
		uint8_t mob_number;
		for(mob_number=0; mob_number<15; mob_number++){
			if (check_mob_ready(mob_number)){
				CANPAGE = (mob_number << 4);

				dlc=CANCDMOB & 0x0F;		// DLC der empfangenen Botschaft lesen
				uint16_t ID =can_get_normal_id();
				switch(ID){
					case 0x280:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id280_data[i] = CANMSG;
						}
						id280_valid = 1;
						break;
					}
					case 0x288:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id288_data[i] = CANMSG;
						}
						id288_valid = 1;
						break;
					}
					case 0x380:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id380_data[i] = CANMSG;
						}
						id380_valid = 1;
						break;
					}
					case 0x480:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id480_data[i] = CANMSG;
						}
						id480_valid = 1;
						break;
					}
					case 0x320:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id320_data[i] = CANMSG;
						}
						id320_valid = 1;
						break;
					}
					case 0x420:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id420_data[i] = CANMSG;
						}
						id420_valid = 1;
						break;
					}
					case 0x520:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							id520_data[i] = CANMSG;
						}
						id520_valid = 1;
						break;
					}
					default:{
						for(i=0; i<dlc; i++){
							CANPAGE &= (0xF8 | i);
							temp_data[i] = CANMSG;
						}
					}
				}

				CANSTMOB = 0;
				CANCDMOB = (1<<CONMOB1);
			}
		}
	//}
	can_irq = 0;
}