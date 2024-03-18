#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"

extern int cnt;
extern uint8_t scancode;

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


int(kbd_test_scan)() {
  int r;
  int ipc_status;
  message msg;
  uint8_t irq_set;
  
  if(keyboard_subscribe_int(&irq_set)) 
    return 1;

 while( scancode != ESC_BREAKCODE ) { /* You may want to use a different condition */
 
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    kbc_ih();
                    bool make = (!((scancode & BIT(7)) == BIT(7)));
                    uint8_t size;
                    if(scancode == 0xE0){
                      size = 2;
                    }
                    else{
                      size = 1;
                    }
                    kbd_print_scancode(make,size, &scancode);
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if(keyboard_unsubscribe_int())
    return 1;
  if (kbd_print_no_sysinb(cnt))
    return 1;
  return 0;
}

int(kbd_test_poll)() {

  while( scancode != ESC_BREAKCODE ){
    if (keyboard_read_output(&scancode)) 
      return 1;
    bool make = (!((scancode & BIT(7)) == BIT(7)));
    uint8_t size;
    if(scancode == 0xE0){
      size = 2;
    }
    else{
      size = 1;
    }
    kbd_print_scancode(make,size, &scancode);  
  }

  if (keyboard_restore_int())
    return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
