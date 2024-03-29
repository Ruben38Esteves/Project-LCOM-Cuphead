#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

uint32_t kbd_sysinb_cnt = 0;

extern uint8_t scancode;

int(kbd_test_scan)() {

  int ipc_status;
  uint8_t irq_set;
  message msg;

  if(keyboard_subscribe_interruption(&irq_set) != 0) return 1;

  while(scancode != 0x81){ // a condição de paragem é obter um breakcode da tecla ESC

    if( driver_receive(ANY, &msg, &ipc_status) != 0 ){
        printf("Error");
        continue;
    }

    if(is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbd_ih();
            kbd_print_scancode(!(scancode & BIT(7)), scancode == 0xE0 ? 2 : 1, &scancode);
          }
      }
    }
  }
  if(kbd_print_no_sysinb(kbd_sysinb_cnt)) return 1;
  if(keyboard_unsubscribe_interruption()) return 1;
  return 0;
}

int(kbd_test_poll)() {
  
  while(scancode != 0x81){
    kbd_ih();
    kbd_print_scancode(!(scancode & BIT(7)), scancode == 0xE0 ? 2 : 1, &scancode);
  }
  if(kbd_print_no_sysinb(kbd_sysinb_cnt)) return 1;
  if(keyboard_restore()) return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
