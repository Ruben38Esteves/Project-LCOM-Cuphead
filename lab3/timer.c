#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"

int hook_id = 0;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {


  if (freq > TIMER_FREQ || freq < 19) return 1;

 

  uint8_t controlW;
  if (timer_get_conf(timer, &controlW) != 0) return 1;


  controlW = (controlW & 0x0F) | TIMER_LSB_MSB; 


  uint32_t initVal = TIMER_FREQ / freq;
  uint8_t MSB, LSB;
  util_get_MSB(initVal, &MSB);
  util_get_LSB(initVal, &LSB);
  
  uint8_t selected;      
  switch (timer) {  
    case 0: 
      controlW |= TIMER_SEL0;
      selected = TIMER_0;
      break;
    case 1:
      controlW |= TIMER_SEL1;
      selected = TIMER_1;
      break;
    case 2:
      controlW |= TIMER_SEL2;
      selected = TIMER_2;
      break;
    default:
      return 1;
  }


  if (sys_outb(TIMER_CTRL, controlW) != 0) return 1;


  if (sys_outb(selected, LSB) != 0) return 1;


  if (sys_outb(selected, MSB) != 0) return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_num) {
  if( bit_num == NULL) return 1; 
  *bit_num = BIT(hook_id);     

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != 0) return 1; 
  return 0;
}

int (timer_unsubscribe_int)() {
  
  if (sys_irqrmpolicy(&hook_id) != 0) return 1; 
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /*To be implemented by the students*/
  if (st == NULL || timer > 2 || timer < 0) return 1;
  uint8_t RBC = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
  if (sys_outb(TIMER_CTRL, RBC) != 0) return 1;      
  if (util_sys_inb(TIMER_0 + timer, st)) return 1;    
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
/*To be implemented by the students*/
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

  if (timer_print_config(timer, field, data) != 0) return 1;
  return 0;
}
