// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "gcard.h"
#include "keyboard.h"

extern vbe_mode_info_t current_mode;
extern uint8_t scancode;
extern int counter;

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

uint32_t get_colour_for_pattern(int i, int j, uint32_t first, uint8_t step){
  // get originals
  uint8_t red = 0xFF & (first >> 16);
  uint8_t green = 0xFF & (first >> 8);
  uint8_t blue = 0xFF & first;
  // calc new values
  red = (red + i * step) % (1 << current_mode.RedMaskSize);
  green = (green + j * step) % (1 << current_mode.GreenMaskSize);
  blue = (blue + (i+j) * step) % (1 << current_mode.BlueMaskSize);

  uint32_t new_colour = (red << 16) | (green << 8) | blue;
  return new_colour;
}

int (wait_for_esc)() {

  int ipc_status;
  message msg;
  uint8_t keyboard_mask;

  if (keyboard_subscribe_interruption(&keyboard_mask) != 0) return 1;

  while (scancode != 0x81){
    if (driver_receive(ANY, &msg, &ipc_status) != 0) { 
      printf("driver_receive failed");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & keyboard_mask) 
            kbd_ih();
            break;
        default:
          break; 
      }
    }
  }

  if (keyboard_unsubscribe_interruption() != 0) return 1;
  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(set_graphics_mode(mode)){
    printf("Failed to set graphics mode");
    return 1;
  }
  sleep(delay);
  if(vg_exit()){
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  if(map_video_memory(mode)){
    printf("Failed to map video memory");
    return 1;
  }
  if(set_graphics_mode(mode)){
    printf("Failed to set graphics mode");
    return 1;
  }
  uint32_t new_colour;
  if(normalize_colour(color, &new_colour)){
    printf("Failed to normalize colour");
    return 1;
  }
  if(vg_draw_rectangle(x,y,width,height,new_colour)){
    printf("Failed to draw rectangle");
    return 1;
  }
  if(wait_for_esc()){
    printf("Failed in waiting for ESC");
    return 1;
  }
  if(vg_exit()){
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(map_video_memory(mode)){
    printf("Failed to map video memory");
    return 1;
  }
  if(set_graphics_mode(mode)){
    printf("Failed to set graphics mode");
    return 1;
  }
  uint32_t color;
  uint16_t rec_width = current_mode.XResolution / no_rectangles;
  uint16_t rec_height = current_mode.YResolution / no_rectangles;
  for(int i = 0; i < no_rectangles; i++){
    for(int j = 0; j < no_rectangles; j++){
      if(mode == 0x105){
        color = (first + (j * no_rectangles + i) * step) % (1 << current_mode.BitsPerPixel);
      }else{
        color = get_colour_for_pattern(i,j,first,step);
      }
      if(vg_draw_rectangle(i*rec_width,j*rec_height,rec_width,rec_height,color)){
        printf("Failed to draw rectangle");
        return 1;
      }
    }
  }
  if(wait_for_esc()){
    printf("Failed in waiting for ESC");
    return 1;
  }
  if(vg_exit()){
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

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
