#include <lcom/lcf.h>

#include <stdint.h>

int counter_KBC = 0;


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  /* To be implemented by the students */
  if(lsb==NULL) {return 1;}
  *lsb = val & 0xFF;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  if(msb==NULL) {return 1;}
  *msb=(val>>8)&0xFF;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  /* To be implemented by the students */
  if(value==NULL) {return 1;}
  uint32_t val;
  int r = sys_inb(port, &val);
  counter_KBC++;
  *value = 0xFF & val;
  return r;
}
