#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter = 0;
int hook_id = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  if(freq>TIMER_FREQ || freq<19) {return 1;}
  uint8_t controlW;
  if(timer_get_conf(timer, &controlW)) {return 1;}
  controlW = (controlW & 0x0F) | BIT(5) | BIT(4);
  
  if(timer==1) {controlW |= BIT(6);}
  if(timer==2) {controlW |= BIT(7);}
  
  uint16_t count = TIMER_FREQ / freq;
  uint8_t LSB, MSB;
  if(util_get_LSB(count, &LSB)) {return 1;}
  if(util_get_MSB(count, &MSB)) {return 1;}

  if(sys_outb(0x43,controlW)) {return 1;}
  if(sys_outb(0x40+timer, LSB)) {return 1;}
  if(sys_outb(0x40+timer, MSB)) {return 1;}

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
    if(bit_no==NULL) {return 1;}
    *bit_no = BIT(hook_id);

    if(sys_irqsetpolicy(0,IRQ_REENABLE, &hook_id)){return 1;}
    return 0;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  if(sys_irqrmpolicy(&hook_id)) {return 1;}

  return 0;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  if(st==NULL || timer>2) {return 1;}
  uint8_t rbc = BIT(7) | BIT(6) | BIT(5) | BIT(timer+1);
  if(sys_outb(0x43,rbc)) {return 1;}
  if(util_sys_inb(0x40+timer, st)) {return 1;}
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  /* To be implemented by the students */
  union timer_status_field_val data;
  switch (field)
  {
  case tsf_all:
    data.byte=st;
    break;
  case tsf_initial:
    st=(st>>4) & 0x03;
    if(st==1) {data.in_mode=LSB_only;}
    else if(st==2) {data.in_mode=MSB_only;}
    else if(st==3) {data.in_mode=MSB_after_LSB;}
    else {data.in_mode=INVAL_val;}
    break;
  case tsf_mode:
    st=(st>>1) & 0x07;
    if(st==6) {data.count_mode=2;}
    if(st==7) {data.count_mode=3;}
    else {data.count_mode=st;}
    break;
  case tsf_base:
      data.bcd = st & BIT(0);
      break;
  
  default:
    break;
  }

  if(timer_print_config(timer, field, data)) {return 1;}

  return 0;

}
