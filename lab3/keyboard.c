#include <keyboard.h>

int hook_id = 1;
uint8_t scancode;


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
  int flag = util_sys_inb(KBC_ST_REG, status);
  return flag;
}

int (keyboard_read_output)(uint8_t* output){
  int attempts = MAX_ATTEMPTS;
  while(attempts > 0){
    uint8_t status;
    if(keyboard_get_status(&status)) 
      return 1;

    if ((status & KBC_OBF) == KBC_OBF){
      if (((status & KBC_TIMEOUT) == KBC_TIMEOUT) || ((status & KBC_PARITY) == KBC_PARITY)){
        return 1;
      }
      if(util_sys_inb(KBC_OUT_BUF, output)) 
        return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    attempts--;
  }

  return 1;
}

void (kbc_ih)(){
  if(keyboard_read_output(&scancode)) 
    return;
}

int (keyboard_write_cmd)(uint8_t port ,uint8_t cmd){
  int attempts = MAX_ATTEMPTS;
  while(attempts > 0){
    uint8_t status;
    if(keyboard_get_status(&status)) 
      return 1;

    if ((status & KBC_IBF) == 0){
      if(sys_outb(port, cmd)) 
        return 1;
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    attempts--;
  }

  return 1;
}

int (keyboard_restore_int)(){
  uint8_t cmd;

  if (keyboard_write_cmd(KBC_CMD_REG, KBC_READ_CMD))
    return 1;
  if (keyboard_read_output(&cmd))
    return 1;

  cmd |= BIT(0);

  if (keyboard_write_cmd(KBC_CMD_REG, KBC_WRITE_CMD))
    return 1;
  if (keyboard_write_cmd(KBC_OUT_BUF, cmd))
    return 1;

  return 0;
}


