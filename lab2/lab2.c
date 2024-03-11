#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  /* To be implemented by the students */
  u_int8_t st;
  if(!timer_get_conf(timer, &st)) return 1;
  if(!timer_display_conf(timer, st,field)) return 1;

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  if(timer>2) return 1;
  if(!timer_set_frequency(timer,freq) ) return 1;

  return 0;
}

int(timer_test_int)(uint8_t time) {
  /* To be implemented by the students */
  int ipc_status, r;
  int ipc_status;
 4: message msg;
 5:
 6: while( 1 ) { /* You may want to use a different condition */
 7:    /* Get a request message. */
 8:    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
 9:        printf("driver_receive failed with: %d", r);
10:        continue;
11:    }
12:    if (is_ipc_notify(ipc_status)) { /* received notification */
13:        switch (_ENDPOINT_P(msg.m_source)) {
14:            case HARDWARE: /* hardware interrupt notification */				
15:                if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
16:                    ...   /* process it */
17:                }
18:                break;
19:            default:
20:                break; /* no other notifications expected: do nothing */	
21:        }
22:    } else { /* received a standard message, not a notification */
23:        /* no standard messages expected: do nothing */
24:    }
25: }
}
