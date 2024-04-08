
#ifndef _KEYBOARD_H_
#define	_KEYBOARD_H_

#include <lcom/lcf.h>
#include <lcom/lab3.h>

int (kbc_int_subscribe)(uint8_t *bit_no);

int (kbc_int_unsubscribe)();

int (kbc_restore)();

void (kbc_ih)();

#endif /**/
