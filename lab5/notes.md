# FOR THE TEST

## First

Create a global vbe_mode_info_t struct called **current_mode**

## Map video Memory
We need to map the vram that wer are going to use
 - 1 - set the memory of **current_mode** to 0
    - **memset(&current_mode, 0, sizeof(current_mode))** -> returns a pointer
 - 2 - load the info of the current mode to **current_mode**
    - **vbe_get_mode_info(mode, &current_mode)**
 - 3 - get important information from **curretn_mode**
    - X resolution
    - Y resolution
    - Bytes per Pixel
 - 4 - create a srtuc **minix_mem_range** phys_addr
    - phys_addr.mr_base = current_mode.PhysBasePtr
    - phys_addr.mr_limit = mr_base + memory needed for the whole screen (xres * yres) * bytesPP
 - 5 - get permission to write to vram
    - **sys_privctl(SELF, SYS_PRIV_ADD_MEM, &phys_addr)**
 - 6 - map memory
    - vm_map_phys(SELF, (void *) phys_addr.mr_base, (X_res * Y_res) * BytesPP);

## Set graphics mode
We need to create a struct to send as command:
 - 1 - create a reg86_t struct
 - 2 - set its memory to 0
    - memset(&reg86, 0, sizeof(reg86));
 - 3 - send the command
    - sys_int86(&reg86)

## normalize colour
If the mode wwe are using does not have 32 bits per color, we need to adapt the input
 - **normalize_colour(uint32_t color, uint32_t *new_colour)**
    - *new_colour = color & (BIT(current_mode.BitsPerPixel) - 1)

## Draw a pixel
 - 1 - **int(draw_pixel)(uint16_t x, uint16_t y, uint32_t colour)**
    - check if x and y are valid
 - 2 - **memcpy(&frame_buffer[index], &colour, BytesPP)**
    - index is calculated as **((y * current_mode.XResolution) + x) * BytesPP**
    - bytes per pixel as **(current_mode.BitsPerPixel + 7) / 8**