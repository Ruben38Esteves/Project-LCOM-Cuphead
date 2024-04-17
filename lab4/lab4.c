// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "mouse.h"
#include "timer.c"

extern int idx;
extern struct packet mouse_packet;
extern int counter_timer;




int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {

  int r;
  int ipc_status;
  message msg;
  uint8_t irq_set;


  // if (mouse_enable_data_reporting())
  //  return 1;
  //comentei pq tava a dar warning

  if (mouse_write_cmd(MOUSE_ENABLE_DATA_REPORTING))
    return 1;
  

  if (mouse_subscribe_int(&irq_set))
    return 1;


  while(cnt > 0) { /* You may want to use a different condition */
 
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                    mouse_ih();
                    mouse_bytes_sync();

                    if (idx == 3){
                      idx = 0;
                      cnt--;
                      mouse_generate_packet();
                      mouse_print_packet(&mouse_packet);
                    }
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }

  if (mouse_unsubscribe_int())
    return 1;

  if (mouse_write_cmd(MOUSE_DISABLE_DATA_REPORTING))
    return 1;


    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {

  
  int r;
  int ipc_status;
  message msg;
  uint8_t irq_set_timer;
  uint8_t irq_set_mouse;
  int time = 0; 
  uint32_t timer_freq = sys_hz();


  // if (mouse_enable_data_reporting())
  //  return 1;
  //comentei pq tava a dar warning

  if (mouse_write_cmd(MOUSE_ENABLE_DATA_REPORTING))
    return 1;
  

  if (mouse_subscribe_int(&irq_set_mouse))
    return 1;

  if (timer_subscribe_int(&irq_set_timer))
    return 1;


  while(time != idle_time) { /* You may want to use a different condition */
 
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */

                if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
                    timer_int_handler();
                    if (counter_timer % timer_freq == 0){
                      time++;
                    }
                }

                if (msg.m_notify.interrupts & irq_set_mouse) { /* subscribed interrupt */
                    mouse_ih();
                    mouse_bytes_sync();

                    if (idx == 3){
                      idx = 0;
                      mouse_generate_packet();
                      mouse_print_packet(&mouse_packet);
                    }
                    
                    counter_timer = 0;
                    time = 0;
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
  }

  if (mouse_unsubscribe_int())
    return 1;

  if (timer_unsubscribe_int())
    return 1;

  if (mouse_write_cmd(MOUSE_DISABLE_DATA_REPORTING))
    return 1;


    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
