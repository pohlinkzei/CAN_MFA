
#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

typedef struct menu_item{
	char text[17];
	struct menu_item* child;
	struct menu_item* parent;
	struct menu_item* next;
	uint8_t num_child;
	uint8_t is_value;
	uint8_t value;
	uint8_t is_switch;
	uint8_t switch_value;
}menu_item_t;

void display_task();
uint8_t get_text_length(char* text, uint8_t max_len);
void display_tuer_open(void);
void display_tuer_closed(void);
void display_menu_init(void);

#endif