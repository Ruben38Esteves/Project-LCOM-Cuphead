// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include <graphics.c>
#include "kbc.h"
#include "keyboard.h"


// Any header files included below this line should have been created by you

extern vbe_mode_info_t info;
extern uint8_t *video_mem;

extern uint8_t scancode;

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
  /* To be completed */
  
  reg86_t reg86p;

  memset(&reg86p, 0, sizeof(reg86p));

  reg86p.ax= 0x4F03; // set ax with 16 bit value
  reg86p.bx = 1<<14|mode; 
  reg86p.intno = 0x10;


  if(sys_int86(&reg86p)==EFAULT) {return 1;}

  sleep(delay);  

  if(vg_exit()) {return 1;}

  return 0;
}


int (wait_ESC)() {

  int ipc_status;
  message msg;
  uint8_t kbd_mask;

  if (kbc_int_subscribe(&kbd_mask) != 0) return 1;

  while (scancode != 0x81){
    if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
      printf("driver_receive failed");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & kbd_mask) 
            kbc_ih();
            break;
        default:
          break; 
      }
    }
  }

  if (kbc_int_unsubscribe() != 0) return 1;
  return 0;
}


int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  /* To be completed */

  if (set_frame_buffer(mode)) {return 1;} 

  if(setting_the_graphic_mode(mode)) {return 1;}


  uint32_t new_color;
  if (normalize_color(color, &new_color) != 0) return 1;


  for(unsigned i = 0; i < height ; i++)
    if (draw_horizontal_line(width, x, y+i, color) != 0) {
      vg_exit();
      return 1;
    }

  if(wait_ESC()) {return 1;}


  if(vg_exit()) {return 1;}

  return 0; 
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
  if (set_frame_buffer(mode)) return 1;

  
  if (setting_the_graphic_mode(mode)) return 1;

  
  int vertical = info.YResolution / no_rectangles;
  int horizontal = info.XResolution / no_rectangles;

  for (int i = 0 ; i < no_rectangles ; i++) {
    for (int j = 0 ; j < no_rectangles ; j++) {

      uint32_t color;

      if (info.MemoryModel == 0x06) {
        uint32_t R = Red(j, step, first);
        uint32_t G = Green(i, step, first);
        uint32_t B = Blue(j, i, step, first);
        color = direct_mode(R, G, B);

      } else {
        color = indexed_mode(j, i, step, first, no_rectangles);
      }

      for(int a = 0; a < vertical ; a++)
        if (draw_horizontal_line(j*horizontal, (i*vertical)+a, horizontal, color) != 0) {
          vg_exit();
          return 1;
        }
    }
  }


  if (wait_ESC()) return 1;

  if (vg_exit() != 0) return 1;

  return 0;
}


int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  if (set_frame_buffer(0x105)) return 1;

  if (setting_the_graphic_mode(0x105)) return 1;
  
  if(draw_xpm(x,y,xpm)) {return 1;}

  if(wait_ESC()) {return 1;}

  if(vg_exit()) {return 1;}

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}

