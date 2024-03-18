#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

int read_status(uint8_t* stat);

int write_KBC(int port, uint8_t command);

int read_KBC(int port, uint8_t *output);



#endif 