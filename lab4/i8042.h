#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_


#define ESC_BREAKCODE 0x81


#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60



#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define KBC_AUX BIT(5) // 0 TECLADO, 1 RATO
#define KBC_TIMEOUT BIT(6)
#define KBC_PARITY BIT(7)

#define MOUSE_IRQ 12

#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_LB BIT(0)
#define MOUSE_X_MSB BIT(4)
#define MOUSE_Y_MSB BIT(5)
#define MOUSE_X_OVFL BIT(6)
#define MOUSE_Y_OVFL BIT(7)

#define MOUSE_WRITE_BYTE 0xD4
#define MOUSE_ENABLE_DATA_REPORTING 0xF4
#define MOUSE_DISABLE_DATA_REPORTING 0xF5


#define ACK 0xFA    
#define NACK 0xFE
#define ERROR 0xFC


#define MAX_ATTEMPTS 10
#define DELAY_US 20000

#endif /* _LCOM_I8042_H */
