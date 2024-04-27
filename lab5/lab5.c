// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "graphics.h"
#include "keyboard.h"
#include "timer.c"

extern uint8_t scancode;
extern vbe_mode_info_t info;



int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (set_graphic_mode(mode) != 0)
    return 1;

  sleep(delay);

  if (vg_exit() != 0) //retornar ao modo texto
    return 1;

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {


  if (set_buffer(mode) != 0)
    return 1;

  if (set_graphic_mode(mode) != 0)
    return 1;

  if (vg_draw_rectangle(x, y, width, height, color) != 0)
    return 1;

  int ipc_status;
  message msg;
  uint8_t kbd_irq_set;

  if(keyboard_subscribe_int(&kbd_irq_set) != 0) 
    return 1;

  while (scancode != ESC_BREAKCODE){
      if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
        printf("driver_receive failed");
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: 
              if (msg.m_notify.interrupts & kbd_irq_set) {
                  kbc_ih();
                  break;
              }
            default:
                break; 
        }
      }
  }

  if(keyboard_unsubscribe_int() != 0)
    return 1;

  if (vg_exit() != 0)
    return 1;


  return 0;

}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
    if (set_buffer(mode) != 0)
    return 1;

  if (set_graphic_mode(mode) != 0)
    return 1;

  
  int y_rectangles = info.YResolution / no_rectangles;
  int x_rectangles = info.XResolution / no_rectangles;

  for (int i = 0 ; i < no_rectangles ; i++) {
    for (int j = 0 ; j < no_rectangles ; j++) {

      uint32_t color;

      if (info.MemoryModel == 0x06) {
        uint32_t R = Red(j, step, first);
        uint32_t G = Green(i, step, first);
        uint32_t B = Blue(j, i, step, first);
        
        color = direct_mode(R, G, B);

      } 
      else {
        color = indexed_mode(j, i, step, first, no_rectangles);
      }

      if (vg_draw_rectangle(j * x_rectangles, i * y_rectangles, x_rectangles, y_rectangles, color) != 0){
        vg_exit();
        return 1;
      }
    
    }
  }



  int ipc_status;
  message msg;
  uint8_t kbd_irq_set;

  if(keyboard_subscribe_int(&kbd_irq_set) != 0) 
    return 1;

  while (scancode != ESC_BREAKCODE){
      if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
        printf("driver_receive failed");
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: 
              if (msg.m_notify.interrupts & kbd_irq_set) {
                  kbc_ih();
                  break;
              }
            default:
                break; 
        }
      }
  }

  if(keyboard_unsubscribe_int() != 0)
    return 1;

  if (vg_exit() != 0)
    return 1;


  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  if (set_buffer(0x105) != 0)
    return 1;

  if (set_graphic_mode(0x105) != 0)
    return 1;

  if (draw_xpm(xpm, x, y) != 0)
    return 1;

  int ipc_status;
  message msg;
  uint8_t kbd_irq_set;

  if(keyboard_subscribe_int(&kbd_irq_set) != 0) 
    return 1;

  while (scancode != ESC_BREAKCODE){
      if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
        printf("driver_receive failed");
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: 
              if (msg.m_notify.interrupts & kbd_irq_set) {
                  kbc_ih();
                  break;
              }
            default:
                break; 
        }
      }
  }

  if(keyboard_unsubscribe_int() != 0)
    return 1;

  if (vg_exit() != 0)
    return 1;


  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  if (set_buffer(0x105) != 0)
    return 1;

  if (set_graphic_mode(0x105) != 0)
    return 1;


  int ipc_status;
  message msg;
  uint8_t kbd_irq_set, timer_irq_set;

  if (keyboard_subscribe_int(&kbd_irq_set) != 0) 
    return 1;

  if (timer_set_frequency(0, fr_rate) != 0)
    return 1;

  if (timer_subscribe_int(&timer_irq_set) != 0)
    return 1;

  if (draw_xpm(xpm, xi, yi) != 0)
    return 1;

  bool horizontal;

  if (xi == xf)
    horizontal = false;
  else
    horizontal = true;


  while (scancode != ESC_BREAKCODE){
      if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
        printf("driver_receive failed");
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: 
              if (msg.m_notify.interrupts & kbd_irq_set) {
                  kbc_ih();
                  break;
              }

              if (msg.m_notify.interrupts & timer_irq_set) {
                  if(vg_draw_rectangle(xi, yi, 200, 200, 0) != 0)
                    return 1;

                  if (horizontal){
                    xi += speed;
                    if (xi > xf) //parar o movimento
                      xi = xf;
                  }
                  else{
                    yi += speed;
                    if (yi > yf) //parar o movimento
                      yi = yf;
                  }

                  if(draw_xpm(xpm, xi, yi) != 0)
                    return 1;

                  break;
              }
            default:
                break; 
        }
      }
  }

  if(timer_unsubscribe_int() != 0)
    return 1;

  if(keyboard_unsubscribe_int() != 0)
    return 1;

  if (vg_exit() != 0)
    return 1;


  return 0;
  
}

/*int(video_test_controller)() {
  // To be completed 
  printf("%s(): under construction\n", __func__);

  return 1;
}
*/

