
#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_
#if ENABLE_SETTINGS
typedef struct menu_item{
	const char text[17];
	const struct menu_item* child;
	const struct menu_item* parent;
	const struct menu_item* next;
	const uint8_t num_child;
	const uint8_t is_value;
	uint8_t value;
	const uint8_t is_switch;
	uint8_t switch_value;
//	uint8_t* mem_address;
}menu_item_t;

menu_item_t* display_settings_next(menu_item_t* item);
menu_item_t* display_settings_child(menu_item_t* item);
menu_item_t* display_settings_nth_child(menu_item_t* item, uint8_t n);
menu_item_t* display_settings_parent(menu_item_t* item);
void display_settings_set_value(menu_item_t* setting, uint8_t val);
void display_settings_set_switch_value(menu_item_t* setting, uint8_t val);
uint8_t display_settings_get_value(menu_item_t* setting);
uint8_t display_settings_get_switch_value(menu_item_t* setting);
void display_settings_save_value(void);

extern const menu_item_t settings_menu;
extern const menu_item_t settings_temperatures;
extern const menu_item_t settings_mfa_values;
extern const menu_item_t settings_options;
extern const menu_item_t settings_cal_voltage;
extern const menu_item_t settings_cal_k58b;
extern volatile menu_item_t *current_entry;
extern volatile uint8_t field_position;
extern volatile uint8_t max_field_position;
/*
extern  menu_item_t settings_cal_oil_temperature;
extern  menu_item_t settings_cal_manifold;
extern  menu_item_t settings_cal_gearbox_temperature;
extern  menu_item_t settings_cal_ambient_temperature;
extern  menu_item_t settings_cal_voltage; // 171
extern  menu_item_t settings_cal_speed; // 169
extern  menu_item_t settings_cal_consumption;
extern  menu_item_t settings_cal_k15_delay;
extern  menu_item_t settings_cal_k58b_off_val;
extern  menu_item_t settings_cal_k58b_on_val;
extern  menu_item_t settings_cal_can_mode;
extern  menu_item_t settings_cal_startstop_enabled;
*/
#endif

void display_task();
uint8_t get_text_length(char* text, uint8_t max_len);
void display_tuer_open(void);
void display_tuer_closed(void);
void display_menu_init(void);

#endif