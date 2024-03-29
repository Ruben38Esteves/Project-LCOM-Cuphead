#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include "keyboard.h"

uint8_t scancode = 0;

#define KBD_IRQ 1
static int hook_id = KBD_IRQ;


int (keyboard_subscribe_interruption)(uint8_t *bit_no){
	*bit_no = BIT(hook_id);
	if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)) return 1;
	return 0;
}

int (keyboard_unsubscribe_interruption)(){
    if(sys_irqrmpolicy(&hook_id)) return 1;
	return 0;
}

void (kbd_ih)(){
  if(keyboard_read_output(0x60, &scancode)){
    printf("error reading scancode");
  }
}

//a diferença entre um makecode e um breakcode é o 7o bit
void (print_scancode_type)(uint8_t scancode){
  if(scancode & BIT(7)){
    printf("breakcode: %02x\n", scancode);
  }else{
    printf("makecode: %02x\n", scancode);
  }
}


int (keyboard_write_command)(uint8_t port, uint8_t cmd){

  uint8_t attempts = 10;
  uint8_t status;

  while(attempts){

    if(util_sys_inb(0x64, &status)){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    if(status & BIT(1)){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    if(sys_outb(0x60, cmd)){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    return 0;
  }
  return 1;
}

int (keyboard_read_output)(uint8_t port, uint8_t *output){
  uint8_t attempts = 10;
  uint8_t status;

  while(attempts){

    if(util_sys_inb(0x64, &status)){
      return 1;
    }

    if(!(status & BIT(0))){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    if(status & BIT(6)){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    if(status & BIT(7)){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;
    }

    if(util_sys_inb(0x60, output)){
      attempts--;
      tickdelay(micros_to_ticks(20000));
      continue;

    }
    return 0;
  }
  return 1;
}

int keyboard_restore(){

  uint8_t old_config;

  if(keyboard_write_command(0x64, 0x20)) //avisar de leitura
    return 1;
  if(keyboard_read_output(0x64, &old_config))
    return 1;

  uint8_t new_config = old_config & BIT(0);

  if(keyboard_write_command(0x64, 0x60)) //avisar de escrita
    return 1;
  if(keyboard_write_command(0x64, new_config))
    return 1;
  return 0;

}
