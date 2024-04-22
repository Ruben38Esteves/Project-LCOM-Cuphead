#include <lcom/lcf.h>
#include "graphs.h"
#include <math.h>

vbe_mode_info_t info;
uint8_t *video_mem;

int (set_graphic_mode)(uint16_t submode) {
    reg86_t r;
    memset(&r, 0, sizeof(r));
    r.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
    r.bx = 1<<14|submode; // set bit 14: linear framebuffer
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


// Função que inicializa o modo gráfico
int (set_frame_buffer)(uint16_t mode){

    //vou buscar a informação do modo
    memset(&info, 0, sizeof(info));
    if(vbe_get_mode_info(mode, &info)) 
        return 1;

    // cálculo do tamanho do frame buffer
    unsigned int bytes_per_pixel = (info.BitsPerPixel + 7) / 8;
    unsigned int vram_size = info.XResolution * info.YResolution * bytes_per_pixel;
    
    // endereços físicos
    struct minix_mem_range mr;
    mr.mr_base = info.PhysBasePtr; //inicio
    mr.mr_limit = mr.mr_base + vram_size; // fim 
    int r;
    
    //mem fisica
    if ((r=sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))!=0) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
    }
    //mem virtual
    video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);
    if(video_mem == MAP_FAILED){
        panic("couldn’t map video memory");
        return 1;
    }
    return 0;
}

// Função que desenha um píxel de uma dada cor
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  
  if(x > info.XResolution || y > info.YResolution) 
    return 1;
  
  unsigned bpp = (info.BitsPerPixel + 7) / 8;

  unsigned int idx = (info.XResolution * y + x) * bpp;

  if (memcpy(&video_mem[idx], &color, bpp) == NULL) 
    return 1;

  return 0;
}

// Função que desenha uma linha horizontal
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (unsigned i = 0 ; i < len ; i++)   
    if (vg_draw_pixel(x+i, y, color) != 0) return 1;
  return 0;
}

// Função que normaliza a cor
int normalize_color(uint32_t color, uint32_t *new_color) {
  if (info.BitsPerPixel == 32) {
    *new_color = color;
  } else {
    *new_color = color & (BIT(info.BitsPerPixel) - 1);
  }
  return 0;
}


// Funções auxiliares da video_test_pattern()

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
