// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "kbc.h"
#include "timer.c"

// Any header files included below this line should have been created by you
/* para as três primeiras funções */
extern struct packet mouse_packet;
extern uint8_t byte_index;
extern int timer_counter;

/* para a função mouse_test_gesture() */
typedef enum {
  START,
  UP,
  VERTEX,
  DOWN,
  END
} SystemState;

SystemState state = START;
uint16_t x_len_total = 0;

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
    /* To be completed */
    int ipc_status;
  message msg;
  uint8_t mouse_mask;


  if (mouse_subscribe_int(&mouse_mask)  ) return 1;

  // Ativar o report de dados do rato com
  // A -> Função implementada de raíz
  // B -> Função dada pelos professores
  if (mouse_write(0xF4)  ) return 1; // A
  //if (mouse_enable_data_reporting()  ) return 1; // B

  while (cnt) { 

    if (driver_receive(ANY, &msg, &ipc_status)  ){
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: 
          if (msg.m_notify.interrupts & mouse_mask){  
            mouse_ih();                               
            mouse_sync_bytes();                       
            if (byte_index == 3) {                   
              mouse_bytes_to_packet();               
              mouse_print_packet(&mouse_packet);      
              byte_index = 0;
              cnt--;
            }
          }
          break;
      }
    }
  }
  
  if (mouse_write(0xF5)) return 1;

  if (mouse_unsubscribe_int()) return 1;
 
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    int ipc_status;
  message msg;
  uint8_t seconds = 0;
  uint8_t mouse_mask = 0, timer_mask = 0; 
  uint16_t timer_frequency = sys_hz();


  if (mouse_subscribe_int(&mouse_mask)  ) return 1;
  if (timer_subscribe_int(&timer_mask)  ) return 1;

  // Ativar o report de dados do rato com
  // A -> Função implementada de raíz
  // B -> Função dada pelos professores
  if (mouse_write(0xF4)  ) return 1; // A
  //if (mouse_enable_data_reporting()  ) return 1; // B

  while (seconds < idle_time) { 

    if (driver_receive(ANY, &msg, &ipc_status)  ){
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: 

          if (msg.m_notify.interrupts & timer_mask) { 
            timer_int_handler();
            if (timer_counter % timer_frequency == 0) seconds++;
          }

          if (msg.m_notify.interrupts & mouse_mask){  
            mouse_ih();                               
            mouse_sync_bytes();                       
            if (byte_index == 3) {                    
              mouse_bytes_to_packet();                
              mouse_print_packet(&mouse_packet);      
              byte_index = 0;
            }
            seconds = 0;
            timer_counter = 0;
          }
      }
    }
  }

  if (mouse_write(0xF5)  ) return 1;

  if (timer_unsubscribe_int()  ) return 1;
  if (mouse_unsubscribe_int()  ) return 1;

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
     while (cnt) { 

        if (mouse_write(0xEB)) 
          return 1;
        mouse_ih();                                     
        mouse_sync_bytes();                               
        if (byte_index == 3) {                            
            mouse_bytes_to_packet();                      
            mouse_print_packet(&mouse_packet);           
            byte_index = 0;
            cnt--;
            tickdelay(micros_to_ticks(period * 1000));    
        }
    }
    uint8_t cmd = minix_get_dflt_kbc_cmd_byte();             
    if (write_KBC_command(0x64, 0x60)) 
      return 1; 
    if (write_KBC_command(0x60, cmd)) 
      return 1;  
    return 0;
}

