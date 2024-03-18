#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define OUT_BUF 0x60 

#define STATUS_REG 0x64 

#define KEYBOARD_IRQ 1 

#define PARITY BIT(7)
#define TIMEOUT BIT(6)

#define AUX BIT(5)

#define IN_BUFF BIT(1)
#define OUT_BUFF BIT(0)

#define READ_COMMAND 0x20
#define write_COMMAND 0x60

#define DELAY 20000






#endif /* _LCOM_I8042_H */