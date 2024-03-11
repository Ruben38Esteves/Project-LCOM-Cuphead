#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"



int (driver_receive)() {
    int counter = 0;
    
    #define LAB3
    sys_inb(...);
    #ifdef LAB3
    cnt++;
    #endif

    return counter;
}