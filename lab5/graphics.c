#include "graphics.h"

vbe_mode_info_t info;
uint8_t *video_mem;

int (set_graphic_mode)(uint16_t mode) {
    reg86_t r;
    memset(&r, 0, sizeof(r));
    r.ax = 0x4F02;
    r.bx = BIT(14) | mode;
    r.intno = 0x10;
    if( sys_int86(&r) != OK ) {
        printf("set_vbe_mode: sys_int86() failed \n");
        return 1;
    }
    return 0;
}

/*
typedef struct {
    uint16_t ModeAttributes;
    [...]
    uint16_t XResolution;
    uint16_t YResolution;
    [...]
    uint8_t BitsPerPixel;
    [...]
    uint8_t RedMaskSize;
    uint8_t RedFieldPosition;
    [...]
    uint8_t RsvdMaskSize;
    uint8_t RsvdFieldPosition;
    [...]
    uint32_t PhysBasePtr;
    [...]
} vbe_mode_info_t;
*/

int (set_buffer)(uint16_t mode) {

    memset(&info, 0, sizeof(info));
    if(vbe_get_mode_info(mode, &info) != 0)
        return 1;


    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = info.PhysBasePtr;
    unsigned int vram_size = info.XResolution * info.YResolution * (info.BitsPerPixel + 7) / 8; //adicinar 7 (arredondamento para int)       

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;
    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
    }
        

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED){
        panic("couldn’t map video memory");
        return 1;
    }
    
    return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
    if(x > info.XResolution || y > info.YResolution) 
        return 1;
  
    unsigned bpp = (info.BitsPerPixel + 7) / 8;

    unsigned int idx = (info.XResolution * y + x) * bpp;

    memcpy(&video_mem[idx], &color, bpp);

    return 0;

}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    if(x > info.XResolution || y > info.YResolution) 
        return 1;

    for(int i = 0; i < len; i++){
        if(vg_draw_pixel(x + i, y, color) != 0)
            return 1;
    }

    return 0;

}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    if(x > info.XResolution || y > info.YResolution) 
        return 1;

    for(int i = 0; i < height; i++){
        if(vg_draw_hline(x, y + i, width, color) != 0)
            return 1;
    }

    return 0;
}

int (draw_xpm)(xpm_map_t xmap, uint16_t x, uint16_t y){

  xpm_image_t img;
  uint8_t *map;

  uint16_t 	width, height;
  map = xpm_load(xmap, XPM_INDEXED, &img);

  width = img.width;
  height = img.height;

  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      if(vg_draw_pixel(x + j, y + i, map[i * width + j]) != 0)
        return 1;
    }
  }

  return 0;
}

// Funções auxiliares video_test_pattern()

uint32_t (Red)(unsigned j, uint8_t step, uint32_t first) {
  return (R(first) + j * step) % (1 << info.RedMaskSize);
}

uint32_t (Green)(unsigned i, uint8_t step, uint32_t first) {
  return (G(first) + i * step) % (1 << info.GreenMaskSize);
}

uint32_t (Blue)(unsigned j, unsigned i, uint8_t step, uint32_t first) {
  return (B(first) + (i + j) * step) % (1 << info.BlueMaskSize);
}

uint32_t (direct_mode)(uint32_t R, uint32_t G, uint32_t B) {
  return (R << info.RedFieldPosition) | (G << info.GreenFieldPosition) | (B << info.BlueFieldPosition);
}

uint32_t (indexed_mode)(uint16_t col, uint16_t row, uint8_t step, uint32_t first, uint8_t n) {
  return (first + (row * n + col) * step) % (1 << info.BitsPerPixel);
}

uint32_t (R)(uint32_t first){
  return ((1 << info.RedMaskSize) - 1) & (first >> info.RedFieldPosition);
}

uint32_t (G)(uint32_t first){
  return ((1 << info.GreenMaskSize) - 1) & (first >> info.GreenFieldPosition);
}

uint32_t (B)(uint32_t first){
  return ((1 << info.BlueMaskSize) - 1) & (first >> info.BlueFieldPosition);
}

