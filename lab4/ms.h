#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "KBC.h"

int(ms_subscribe_int)(uint8_t *bit);
int(ms_unsubscribe_int)();
void (ms_ih)();
void (ms_sync_bytes)();
void (ms_bytes_to_packet)();
int (ms_write)(uint8_t command);
