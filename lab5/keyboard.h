#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>



int (keyboard_subscribe_interruption)(uint8_t *bit_no);

int (keyboard_unsubscribe_interruption)();

void (kbd_ih)();

void (print_scancode_type)(uint8_t scancode);

int (keyboard_write_command)(uint8_t port, uint8_t cmd);

int (keyboard_read_output)(uint8_t port, uint8_t *output);

int (keyboard_restore)();

int (timer_subscribe_interruption)(uint8_t *bit_no);

int (timer_unsubscribe_interruption)();

void (tmr_ih)();


#endif
