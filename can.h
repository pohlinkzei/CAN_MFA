#ifndef CAN_H
#define CAN_H
/*
typedef union {
	uint8_t	ci_b[4] ;
	uint32_t	ci_i ;
} can_ID ;
//*/				
extern uint64_t can_speed_sum;
extern uint32_t can_speed_cnt;				
				
void can_init (void) ;	
void can_init_nocan (void) ;
void can_normal_id(uint16_t id_tag, uint8_t id_flags, uint8_t mask_type );
//void can_extended_id(can_ID id_tag, uint8_t id_flags, uint8_t mask_type );
//uint8_t can_poll_mob(uint8_t mob);
void can_task_nocan(void);
void can_task(void);

#endif