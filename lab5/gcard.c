#include "gcard.h"
#include <lcom/lcf.h>

int(set_graphics_mode)(uint16_t mode) {
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));
  /*
      printf("Error setting memory to 0");
      return 1;
  }
  */
  reg86.intno = 0x10;
  reg86.ah = 0x4F;
  reg86.al = 0x02;
  reg86.bx = mode | BIT(14);
  if (sys_int86(&reg86)) {
    printf("Error starting graphics mode");
    return 1;
  }
  return 0;
}

int(map_video_memory)(uint16_t mode) {
  if(memset(&current_mode, 0, sizeof(current_mode)) == NULL){
    printf("Error setting memory to 0 aaaaaaaaaaaaaaaa");
    return 1;
  }
  if (vbe_get_mode_info(mode, &current_mode)) {
    printf("Couldn't retrieve mode info");
    return 1;
  }
  uint16_t X_res, Y_res;
  uint32_t BytesPP;
  X_res = current_mode.XResolution;
  Y_res = current_mode.YResolution;
  BytesPP = (current_mode.BitsPerPixel + 7) / 8;

  struct minix_mem_range phys_addr;
  phys_addr.mr_base = current_mode.PhysBasePtr;
  phys_addr.mr_limit = phys_addr.mr_base + ((X_res * Y_res) * BytesPP);
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &phys_addr)) {
    printf("Physical memory allocation error\n");
    return 1;
  }

  frame_buffer = vm_map_phys(SELF, (void *) phys_addr.mr_base, (X_res * Y_res) * BytesPP);
  if (frame_buffer == MAP_FAILED) {
    return 1;
  }
  return 0;
}

int normalize_colour(uint32_t color, uint32_t *new_colour) {
  if (current_mode.BitsPerPixel == 32) {
    *new_colour = color;
  }
  else {
    *new_colour = color & (BIT(current_mode.BitsPerPixel) - 1);
  }
  return 0;
}

int(draw_pixel)(uint16_t x, uint16_t y, uint32_t colour) {
  if (x > current_mode.XResolution || y > current_mode.YResolution) {
    printf("Invalid coordinates");
    return 0;
  }
  uint32_t BytesPP = (current_mode.BitsPerPixel + 7) / 8;
  uint32_t index = ((y * current_mode.XResolution) + x) * BytesPP;
  if (memcpy(&frame_buffer[index], &colour, BytesPP) == NULL) {
    printf("Clouldn't copy colour to index");
    return 1;
  }
  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t i = 0; i < len; i++) {
    if (draw_pixel(x + i, y, color)) {
      printf("Failed to draw pixel in line");
      return 1;
    }
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color)) {
      printf("Failed to draw line in ractangle");
      return 1;
    }
  }
  return 0;
}
