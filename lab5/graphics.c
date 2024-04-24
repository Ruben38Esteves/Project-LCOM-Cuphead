

vbe_mode_info_t info;
uint8_t *video_mem;





int (setting_the_graphic_mode)(uint16_t mode) {
    reg86_t reg86p;

    memset(&reg86p, 0, sizeof(reg86p));
    
    reg86p.intno = 0x10;
    reg86p.ax = 0x4F02; 
    reg86p.bx = 1<<14;
    reg86p.bx |= mode;

    if(sys_int86(&reg86p)==EFAULT) {return 1;}

    return 0;
}

int (draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  
  if(x > info.XResolution || y > info.YResolution) 
    return 1;
  
  unsigned bpp = (info.BitsPerPixel + 7) / 8;

  unsigned int idx = (info.XResolution * y + x) * bpp;

  if (memcpy(&video_mem[idx], &color, bpp) == NULL) 
    return 1;

  return 0;
}


int (draw_horizontal_line)(uint16_t len, uint16_t x, uint16_t y, uint32_t color) {
  for (unsigned i = 0 ; i < len ; i++)   
    if (draw_pixel(x+i, y, color) != 0) return 1;
  return 0;
}


int normalize_color(uint32_t color, uint32_t *new_color) {
  if (info.BitsPerPixel == 32) {
    *new_color = color;
  } else {
    *new_color = color & (BIT(info.BitsPerPixel) - 1);
  }
  return 0;
}



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

// Funções auxiliares da video_test_pattern()

uint32_t (R)(uint32_t first){
  return ((1 << info.RedMaskSize) - 1) & (first >> info.RedFieldPosition);
}

uint32_t (G)(uint32_t first){
  return ((1 << info.GreenMaskSize) - 1) & (first >> info.GreenFieldPosition);
}

uint32_t (B)(uint32_t first){
  return ((1 << info.BlueMaskSize) - 1) & (first >> info.BlueFieldPosition);
}

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


int (draw_xpm)(uint16_t x, uint16_t y, xpm_map_t xpm) {
  
  xpm_image_t img; 
  

  uint8_t *colors = xpm_load(xpm, XPM_INDEXED, &img);

  for (int h = 0 ; h < img.height ; h++) {
    for (int w = 0 ; w < img.width ; w++) {
      if (draw_pixel(x + w, y + h, *colors) != 0) return 1;
      colors++;
    }
  }
  
  return 0;
}


