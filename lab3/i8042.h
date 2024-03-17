#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_


#define ESC_BREAKCODE 0x81


#define KBC_ST_REG 0x64
#define KBC_OUT_BUF 0x60


#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define KBC_AUX BIT(5) // 0 TECLADO, 1 RATO
#define KBC_TIMEOUT BIT(6)
#define KBC_PARITY BIT(7)




#define MAX_ATTEMPTS 10
#define DELAY_US 20000

#endif /* _LCOM_I8042_H */
