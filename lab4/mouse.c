#include "mouse.h"

int hook_id_mouse = 3;
uint8_t scancode;
struct packet mouse_packet;
uint8_t mouse_bytes[3];
int idx = 0;

int (mouse_subscribe_int)(uint8_t *bit_no){
  if (bit_no == NULL) return 1;

  *bit_no = BIT(hook_id_mouse);

  int flag = sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse);

  return flag;
}

int (mouse_unsubscribe_int)(){
  int flag = sys_irqrmpolicy(&hook_id_mouse);

  return flag;
}

int (mouse_get_status)(uint8_t* status){
  int flag = util_sys_inb(KBC_ST_REG, status);
  return flag;
}

int (mouse_read_output)(uint8_t* output){
  int attempts = MAX_ATTEMPTS;

  while(attempts > 0){
    uint8_t status;
    if(mouse_get_status(&status)) 
      return 1;

    if ((status & KBC_OBF) == KBC_OBF){
      if (((status & KBC_TIMEOUT) == KBC_TIMEOUT) || ((status & KBC_PARITY) == KBC_PARITY) || ((status & KBC_AUX) == 0)){
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

void (mouse_ih)(){
  if(mouse_read_output(&scancode)) //pode ser só sys_inb?
    return;
}

int (kbc_write_cmd)(uint8_t port ,uint8_t cmd){
  int attempts = MAX_ATTEMPTS;
  while(attempts > 0){
    uint8_t status;
    if(mouse_get_status(&status)) 
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

int (mouse_write_cmd)(uint8_t cmd){
  int attempts = MAX_ATTEMPTS;
  uint8_t acknowledgment_byte;

  while(attempts > 0 && acknowledgment_byte != ACK){
    if(kbc_write_cmd(KBC_CMD_REG, MOUSE_WRITE_BYTE)) 
      return 1;
    if(kbc_write_cmd(KBC_OUT_BUF, cmd))
      return 1;
    if(util_sys_inb(KBC_OUT_BUF, &acknowledgment_byte))
      return 1;

    if (acknowledgment_byte == ACK){
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY_US));
    attempts--;
  }

  return 1;
}

void (mouse_generate_packet)(){
  mouse_packet.bytes[0] = mouse_bytes[0];
  mouse_packet.bytes[1] = mouse_bytes[1];
  mouse_packet.bytes[2] = mouse_bytes[2];

  mouse_packet.rb = (mouse_bytes[0] & MOUSE_RB) != 0; 
  mouse_packet.mb = (mouse_bytes[0] & MOUSE_MB) != 0; 
  mouse_packet.lb = (mouse_bytes[0] & MOUSE_LB) != 0;

  mouse_packet.x_ov = (mouse_bytes[0] & MOUSE_X_OVFL) != 0;
  mouse_packet.y_ov = (mouse_bytes[0] & MOUSE_Y_OVFL) != 0;

  if ((mouse_bytes[0] & MOUSE_X_MSB) != 0)
    mouse_packet.delta_x = mouse_bytes[1] | 0xFF00;
  else
    mouse_packet.delta_x = mouse_bytes[1];

  if ((mouse_bytes[0] & MOUSE_Y_MSB) != 0)
    mouse_packet.delta_y = mouse_bytes[2] | 0xFF00;
  else
    mouse_packet.delta_y = mouse_bytes[2];
}

void (mouse_bytes_sync)(){
  if (idx == 0 && (scancode & BIT(3)) != 0){ // primeiro byte (bit 3 ativado)
    mouse_bytes[0] = scancode;
    idx++;
  }
  else if (idx != 0){
    mouse_bytes[idx] = scancode;
    idx++;
  }
}

