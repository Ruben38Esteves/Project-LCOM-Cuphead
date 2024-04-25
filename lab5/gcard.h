#ifndef _GCARD_H_
#define _GCARD_H_

#include <lcom/lcf.h>

vbe_mode_info_t current_mode;
uint8_t* frame_buffer;

int (set_graphics_mode)(uint16_t mode);
int (map_video_memory)(uint16_t mode);
int (normalize_colour)(uint32_t color, uint32_t *new_color);
int (draw_pixel)(uint16_t x, uint16_t y, uint32_t colour);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint32_t get_colour_for_pattern(int i, int j, uint32_t first, uint8_t step);
int (draw_xpm)(uint16_t x, uint16_t y, uint8_t *colour, uint16_t width, uint16_t height);

#endif
