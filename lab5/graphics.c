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

