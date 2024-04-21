#include "i8042.h"
#include <lcom/lcf.h>
#include <minix/sysutil.h>


int (KBC_read_status)(uint8_t* curr_status); //passar por referencia para usar o sys_inb do util
int (KBC_read_output)(uint8_t port, uint8_t *output,uint8_t aux);
int (KBC_write_comm)(uint8_t comm, uint8_t port);
