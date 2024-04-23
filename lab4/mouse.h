

#ifndef _MOUSE_H_
#define	_MOUSE_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

void (mouse_sync_bytes)();

void (mouse_bytes_to_packet)();

int (mouse_write)(uint8_t command);

#endif /**/
