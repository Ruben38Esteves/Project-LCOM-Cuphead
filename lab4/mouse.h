#ifndef MOUSE_H_
#define MOUSE_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"




int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

int (mouse_get_status)(uint8_t* status);

int (mouse_read_output)(uint8_t* output);

void (mouse_ih)();

int (kbc_write_cmd)(uint8_t port ,uint8_t cmd);

int (mouse_write_cmd)(uint8_t cmd);

void (mouse_generate_packet)();

void (mouse_bytes_sync)();

#endif
