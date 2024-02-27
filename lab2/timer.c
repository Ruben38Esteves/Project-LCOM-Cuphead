#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (timer > 2 || timer < 0) return 1;

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL) return 1;
  if (timer > 2 || timer < 0) return 1;
  uint8_t t = BIT(timer+1);
  uint8_t rb_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | t;
  int flag1 = sys_outb(0x43, rb_cmd);
  int flag2 = util_sys_inb(TIMER_0 + timer, st);

  return (flag1 || flag2);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  if (timer > 2 || timer < 0) return 1;
  union timer_status_field_val val;

  switch(field){

    case tsf_all:
      val.byte = st;
      break;

    case tsf_initial: // alguns erros neste caso
      if ((st & TIMER_LSB) == TIMER_LSB){
        val.in_mode = LSB_only;
      }
      else if ((st & TIMER_MSB) == TIMER_MSB){
        val.in_mode = MSB_only;
      }
      else if ((st & TIMER_LSB_MSB) == TIMER_LSB_MSB){
        val.in_mode = MSB_after_LSB;
      }
      else{
        val.in_mode = INVAL_val;
      }
      break;
    
    case tsf_mode: // alguns erros neste caso
      if ((st & TIMER_RATE_GEN) == TIMER_RATE_GEN){
        val.count_mode = 2;
      }
      if ((st & TIMER_SQR_WAVE) == TIMER_SQR_WAVE){
        val.count_mode = 3;
      }
      break;
    
    case tsf_base:
      val.bcd = (bool) (st & 0x01);
      break;

    default:
      return 1;
  };

  int flag =  timer_print_config(timer, field, val);

  return flag;
}
