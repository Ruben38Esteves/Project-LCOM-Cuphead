#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#define KBD_IRQ 1
static int hook_id = KBD_IRQ;


int keyboard_subscribe_interruption(uint8_t *bit_no){
	*bit_no = hook_id;
	if(sys_irqsetpolicy(KBD_IRQ, IRQ_EXCLUSIVE, &hook_id)) return 1;
	return 0;
}

int keyboard_unsubscribe_interruption(){
    if(sys_irqrmpolicy(&hook_id)) return 1;
	return 0;
}

//a diferença entre um makecode e um breakcode é o 7o bit
void print_scancode_type(uint8_t scancode){
  if(scancode & BIT(7)){
    printf("breakcode: %02x\n", scancode);
  }else{
    printf("makecode: %02x\n", scancode);
  }
}