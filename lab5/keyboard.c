
#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "keyboard.h"
#include "kbc.h"

uint8_t scancode = 0;
int kbc_hook_id = 1;

int (kbc_int_subscribe)(uint8_t *bit_no) {
    if (bit_no == NULL) {return 1;}
    *bit_no = BIT(kbc_hook_id);
    if(sys_irqsetpolicy(1,IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id)) {return 1;}
    return 0;
}

int (kbc_int_unsubscribe)() {
    if(sys_irqrmpolicy(&kbc_hook_id)) {return 1;}
    return 0;
}

void (kbc_ih)() {
    if(read_KBC_output(0x60,&scancode,0)) {}
}

int (kbc_restore)() {
    uint8_t commandByte;

    if (write_KBC_command(0x64, 0x20)) return 1;  
    if(read_KBC_output(0x60,&commandByte,0)) {return 1;}

    commandByte |= BIT(0);

    if(write_KBC_command(0x64,0x60)) {return 1;}
    if(write_KBC_command(0x60, commandByte)) {return 1;}

    return 0;
}
