#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

int keyboard_subscribe_int(uint8_t *bit_num);

int keyboard_unsubscribe_int();

int keyboard_read_scancode(uint8_t *scancode);


#endif 