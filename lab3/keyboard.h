#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"




int (keyboard_subscribe_int)(uint8_t *bit_no);

int (keyboard_unsubscribe_int)();

int (keyboard_get_status)(uint8_t* status);

int (keyboard_read_output)(uint8_t* output);

void (kbc_ih)();

int (keyboard_restore_int)();

int (keyboard_write_cmd)(uint8_t port ,uint8_t cmd);

