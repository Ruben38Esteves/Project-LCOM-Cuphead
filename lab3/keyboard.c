#include <keyboard.h>

int hook_id = 1;

int (keyboard_subscribe_int)(uint8_t *bit_no){
  if (bit_no == NULL) return 1;

  *bit_no = BIT(hook_id);

  int flag = sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);

  return flag;
}

int (keyboard_unsubscribe_int)(){
  int flag = sys_irqrmpolicy(&hook_id);

  return flag;
}

int (keyboard_get_status)(uint8_t* status){
  int flag = util_sys_inb(KBC_ST_REG, &status);
  return flag;
}

int (keyboard_read_output)(uint8_t* output){
  int attempts = MAX_ATTEMPTS;
  while(attempts > 0){
    uint8_t status;
    if(keyboard_get_status(&status)) return 1;

    if ((status & KBC_OBF) == KBC_OBF){
      if ((status & KBC_TIMEOUT == KBC_TIMEOUT) || (status & KBC_PARITY == KBC_PARITY)){
        return 1;
      }
      if(util_sys_inb(KBC_OUT_BUF, &output)) return 1;
      return 0;
    }
    //delay(10000);
    attempts--;
  }

  return 1;
}


