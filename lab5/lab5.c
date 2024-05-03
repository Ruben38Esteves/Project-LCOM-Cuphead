// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "gcard.h"
#include "keyboard.h"
#include "timer.c"

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

int(wait_for_esc)() {

  int ipc_status;
  message msg;
  uint8_t keyboard_mask;

  if (keyboard_subscribe_interruption(&keyboard_mask)) {
    return 1;
  }

  while (scancode != 0x81) {
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

  if (keyboard_unsubscribe_interruption() != 0)
    return 1;
  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (set_graphics_mode(mode)) {
    printf("Failed to set graphics mode");
    return 1;
  }
  sleep(delay);
  if (vg_exit()) {
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  if (map_video_memory(mode)) {
    printf("Failed to map video memory");
    return 1;
  }
  if (set_graphics_mode(mode)) {
    printf("Failed to set graphics mode");
    return 1;
  }
  uint32_t new_colour;
  if (normalize_colour(color, &new_colour)) {
    printf("Failed to normalize colour");
    return 1;
  }
  if (vg_draw_rectangle(x, y, width, height, new_colour)) {
    printf("Failed to draw rectangle");
    return 1;
  }
  if (wait_for_esc()) {
    printf("Failed in waiting for ESC");
    return 1;
  }
  if (vg_exit()) {
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (map_video_memory(mode)) {
    printf("Failed to map video memory");
    return 1;
  }
  if (set_graphics_mode(mode)) {
    printf("Failed to set graphics mode");
    return 1;
  }
  uint32_t color;
  uint16_t rec_width = current_mode.XResolution / no_rectangles;
  uint16_t rec_height = current_mode.YResolution / no_rectangles;
  for (int i = 0; i < no_rectangles; i++) {
    for (int j = 0; j < no_rectangles; j++) {
      if (mode == 0x105) {
        color = (first + (j * no_rectangles + i) * step) % (1 << current_mode.BitsPerPixel);
      }
      else {
        color = get_colour_for_pattern(i, j, first, step);
      }
      if (vg_draw_rectangle(i * rec_width, j * rec_height, rec_width, rec_height, color)) {
        printf("Failed to draw rectangle");
        return 1;
      }
    }
  }
  if (wait_for_esc()) {
    printf("Failed in waiting for ESC");
    return 1;
  }
  if (vg_exit()) {
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (map_video_memory(0x105)) {
    printf("Failed to map video memory");
    return 1;
  }
  if (set_graphics_mode(0x105)) {
    printf("Failed to set graphics mode");
    return 1;
  }
  xpm_image_t image;
  memset(&image, 0, sizeof(image));
  uint8_t *colour = xpm_load(xpm, XPM_INDEXED, &image);
  uint16_t width = image.width;
  uint16_t height = image.height;
  if (draw_xpm(x, y, colour, width, height)) {
    printf("failed to draw xpm");
    return 1;
  }
  if (wait_for_esc()) {
    printf("Failed in waiting for ESC");
    return 1;
  }
  if (vg_exit()) {
    printf("Failed exiting video mode");
    return 1;
  }
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  int ipc_status, r;
  message msg;
  uint8_t keyboard_mask, timer_mask;
  uint8_t vertical_direction;
  if (xi == xf && yi < yf) {
    vertical_direction = 1;
  }
  else if (yi == yf && xi < xf) {
    vertical_direction = 0;
  }
  else {
    return 1;
  }
  if (keyboard_subscribe_interruption(&keyboard_mask)) {
    return 1;
  }
  if (timer_subscribe_int(&timer_mask)) {
    return 1;
  }
  uint32_t freq = 0x000000 | fr_rate;
  if (timer_set_frequency(0, freq)) {
    return 1;
  }
  if (map_video_memory(0x105)) {
    printf("Failed to map video memory");
    return 1;
  }
  if (set_graphics_mode(0x105)) {
    printf("Failed to set graphics mode");
    return 1;
  }

  int16_t original_speed = speed;

  xpm_image_t image;
  memset(&image, 0, sizeof(image));
  uint8_t *colour = xpm_load(xpm, XPM_INDEXED, &image);
  uint16_t width = image.width;
  uint16_t height = image.height;
  if (draw_xpm(xi, yi, colour, width, height)) {
    return 1;
  }

  while (scancode != 0x81) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & keyboard_mask) {
            kbd_ih();
          }
          if (msg.m_notify.interrupts & timer_mask) {

            if (original_speed > 0) {
              if (vg_draw_rectangle(xi, yi, width, height, 0x000000)) {
                return 1;
              }
              if (vertical_direction == 1) {
                yi += speed;
                if (yi > yf) {
                  yi = yf;
                }
              }
              else {
                xi += speed;
                if (xi > xf) {
                  xi = xf;
                }
              }
              if (draw_xpm(xi, yi, colour, width, height)) {
                return 1;
              }
            }
            else {
              if (speed == 0) {
                if (vg_draw_rectangle(xi, yi, width, height, 0x000000)) {
                  return 1;
                }
                if (vertical_direction == 1) {
                  yi += 1;
                }
                else {
                  xi += 1;
                }
                if (draw_xpm(xi, yi, colour, width, height)) {
                  return 1;
                }
                speed = original_speed;
              }
              else {
                speed += 1;
              }
            }
          }
      }
    }
  }
  if (vg_exit()) {
    printf("Failed exiting video mode");
    return 1;
  }
  if (keyboard_unsubscribe_interruption()) {
    return 1;
  }
  if (timer_unsubscribe_int()) {
    return 1;
  }
  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
