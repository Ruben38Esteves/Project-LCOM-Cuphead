#include "keyboard.h" // Include your keyboard header file here

int keyboard_subscribe_int(uint8_t *bit_num) {
    if (bit_num == NULL) return 1; // Check for NULL pointer
    
    *bit_num = KBD_IRQ; // Set the bit number
    
    // Subscribe to keyboard interrupts
    if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
        printf("keyboard_subscribe_int: sys_irqsetpolicy failed\n");
        return 1;
    }
    
    return 0;
}

int keyboard_unsubscribe_int() {
    // Unsubscribe from keyboard interrupts
    if (sys_irqrmpolicy(&hook_id) != OK) {
        printf("keyboard_unsubscribe_int: sys_irqrmpolicy failed\n");
        return 1;
    }
    
    return 0;
}

int keyboard_read_scancode(uint8_t *scancode) {
    // Read the scancode from the keyboard
    if (util_sys_inb(KBD_OUT_BUF, scancode) != OK) {
        printf("keyboard_read_scancode: util_sys_inb failed\n");
        return 1;
    }
    
    return 0;
}
