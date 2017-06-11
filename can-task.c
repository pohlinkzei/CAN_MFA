/*
 * can_task.c
 *
 * Created: 11.09.2016 21:27:22
 *  Author: Hubert
 */ 
#include <inttypes.h>
#include <avr/interrupt.h>
#include "CAN-MFA.h"
#include "can.h"
#include "can-task.h"
uint8_t can_irq = 0; // 

void can_read_data(void);

void can_task(void){
	can_read_data();
	if(K15_PIN & (1<<K15)){
		//read data
		if(id280_valid){
			// calculate rpm
			rpm = (id280_data[3] + ((id280_data[2]) << 8))>>2;
			id280_valid = false;
			can_status |= (1<<ID280);
		}
		if(id288_valid){
			engine_temperature = id288_data[1] - 100;
			speed[CUR] = ((id288_data[3] * 5) >> 2)*10;
			//speed = ((id288_data[3] * 50) + 5)/40;
			id288_valid = false;
			can_status |= (1<<ID288);
		}
		if(id380_valid){
			id380_valid = false;
			can_status |= (1<<ID380);
		}
		if(id480_valid){
			cons_ul[cons_cnt] = id480_data[2] + ((id480_data[3] & 0x7F) << 8);
			if((cons_ul[cons_cnt] < cons_ul[cons_cnt-1])){
				cons_ul[cons_cnt] += 0x8000;
			}
			cons_cnt++;
			if (cons_cnt == 16){
				cons_timer = 0;
				uint32_t cons_ul_delta = cons_ul[15] - cons_ul[0];
				uint32_t cons_time_delta = cons_time[15] - cons_time[0];
				cons_l_h = ((cons_ul_delta * 3600.0) * cal_consumption)/256;
				cons_l_h /= (cons_time_delta * 1000.0);
				if (speed[CUR]>14){
					cons_l_100km[CUR] = (cons_l_h * 100.0)/speed[CUR];
				}
				cons_cnt=0;
			}
			can_status |= (1<<ID480);
			id480_valid = false;
		}
		if(id320_valid){
			can_status |= (1<<ID320);
			id320_valid = false;
		}
		if(id420_valid){
			can_status |= (1<<ID420);
			id420_valid = false;
		}
	}
}

void can_read_data(){
	if(can_irq){
		uint8_t i;
		uint8_t mob_number;
		for(mob_number=0; mob_number<15; mob_number++){
			CANPAGE = (mob_number << 4);
			uint8_t mob_status = can_get_mob_status();
			if(mob_status == MOB_RX_COMPLETED){
				uint16_t ID =can_get_id();
				switch(ID){
					case 0x280:{
						for(i=0; i<8; i++){
							id280_data[i] = CANMSG;
						}
						id280_valid = true;
						break;
					}
					case 0x288:{
						for(i=0; i<8; i++){
							id288_data[i] = CANMSG;
						}
						id288_valid = true;
						break;
					}
					case 0x380:{
						for(i=0; i<8; i++){
							id380_data[i] = CANMSG;
						}
						id380_valid = true;
						break;
					}
					case 0x480:{
						cons_time[cons_cnt] = cons_timer; //get timing for consumption calculation
						for(i=0; i<8; i++){
							id480_data[i] = CANMSG;
						}
						id480_valid = true;
						break;
					}
					case 0x320:{
						for(i=0; i<8; i++){
							id320_data[i] = CANMSG;
						}
						id320_valid = true;
						break;
					}
					case 0x420:{
						for(i=0; i<8; i++){
							id420_data[i] = CANMSG;
						}
						id420_valid = true;
						break;
					}
				}
			}
			Can_config_rx();
			Can_set_dlc(8);
		}
	}
	can_irq = 0;
}
