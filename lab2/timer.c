#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = TIMER0_IRQ;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
	if (freq > TIMER_FREQ || freq < 19) return 1;

	// buscar config antiga e preparar a nova
	uint8_t old_config = 0, new_config = 0;
	if(timer_get_conf(timer, &old_config)) return 1;
	new_config = (old_config & 0x0F) | TIMER_LSB_MSB; 
	
	// is buscar lsb e msb da freq desejada
	uint32_t initial_value = TIMER_FREQ / freq;
  	uint8_t msb = 0, lsb = 0;
	if(util_get_LSB(initial_value, &lsb)) return 1;
	if(util_get_MSB(initial_value, &msb)) return 1;

	// inserir o timer que queremos na nova config
	uint8_t selected_timer;      
	switch (timer) {  
		case 0: 
			new_config |= TIMER_SEL0;
			selected_timer = TIMER_0;
			break;
		case 1:
			new_config |= TIMER_SEL1;
			selected_timer = TIMER_1;
			break;
		case 2:
			new_config |= TIMER_SEL2;
			selected_timer = TIMER_2;
			break;
		default:
			return 1;
	}

	// enviar nova config e avisar de lsb e msb
	if(sys_outb(TIMER_CTRL, new_config)) return 1;

	//enviar lsb e msb
	if(sys_outb(selected_timer, lsb)) return 1;
	if(sys_outb(selected_timer, msb)) return 1;

	return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
	*bit_no = BIT(hook_id);
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)) return 1;
	return 0;
}

int (timer_unsubscribe_int)() {
	if(sys_irqrmpolicy(&hook_id)) return 1;
	return 0;
}

void (timer_int_handler)() {
	counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st){
    if(st == NULL || timer > 2 || timer < 0) return 1;
    uint8_t RBC = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
    if(sys_outb(TIMER_CTRL, RBC)) return 1;
    if(util_sys_inb(TIMER_0 + timer, st)) return 1;
    return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
	
	union timer_status_field_val data;

	switch (field) {
		case tsf_all: 
			data.byte = st; 
			break;

		case tsf_initial:                                       
			st = (st >> 4) & 0x03;

			if (st == 1) data.in_mode = LSB_only;
			else if (st == 2) data.in_mode = MSB_only;
			else if (st == 3) data.in_mode = MSB_after_LSB;
			else data.in_mode = INVAL_val;
			
			break;

		case tsf_mode:
			st = (st >> 1) & 0x07;

			if(st == 6) data.count_mode = 2;
			else if(st == 7) data.count_mode = 3;
			else data.count_mode = st;

			break;
		
		case tsf_base:
			data.bcd = st & TIMER_BCD;
			break;        

		default:
			return 1;
	}

	if (timer_print_config(timer, field, data)) return 1;
	return 0;
}
