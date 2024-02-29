#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq <= 0 || freq > TIMER_FREQ) return 1;

  uint8_t config;
  int flag1 = timer_get_conf(0, &config);
  uint16_t rate = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  int flag2 = util_get_LSB(rate, &lsb);
  int flag3 = util_get_MSB(rate, &msb);

  uint8_t ctrl_word = (0x0F & config) | TIMER_LSB_MSB;

  switch(timer){
    case 0:
      ctrl_word = ctrl_word | TIMER_SEL0;
      break;
    case 1:
      ctrl_word = ctrl_word | TIMER_SEL1;
      break;
    case 2:
      ctrl_word = ctrl_word | TIMER_SEL2;
      break;

    default:
      return 1;
  }

  int flag4 = sys_outb(TIMER_CTRL, ctrl_word);
  int flag5 = sys_outb(TIMER_0 + timer, lsb);
  int flag6 = sys_outb(TIMER_0 + timer, msb);


  return (flag1 || flag2 || flag3 || flag4 || flag5 || flag6);
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
  uint8_t mode = st & 0x0E;

  switch(field){

    case tsf_all:
      val.byte = st;
      break;

    case tsf_initial:
      if ((st & TIMER_LSB_MSB) == TIMER_LSB_MSB){
        val.in_mode = MSB_after_LSB;
      }
      else if ((st & TIMER_LSB) == TIMER_LSB){
        val.in_mode = LSB_only;
      }
      else if ((st & TIMER_MSB) == TIMER_MSB){
        val.in_mode = MSB_only;
      }
      else{
        val.in_mode = INVAL_val;
      }
      break;
    
    case tsf_mode:

      if ((mode & TIMER_SQR_WAVE) == TIMER_SQR_WAVE){
        val.count_mode = 3;
      }
      else if ((mode & TIMER_RATE_GEN) == TIMER_RATE_GEN){
        val.count_mode = 2;
      }

      else val.count_mode = (mode >> 1);

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
