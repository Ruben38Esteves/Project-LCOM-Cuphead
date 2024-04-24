#include <lcom/lcf.h>

#include <stdint.h>


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
	if(lsb == NULL) return 1;
	*lsb = 0xFF & val;
	return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  	if(msb == NULL) return 1;
	*msb = val >> 8;
	return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
	if(value == NULL) return 1;
	uint32_t new_value = 0;
	int returned = sys_inb(port, &new_value);
	#ifdef LAB3
	extern uint32_t kbd_sysinb_cnt;
	kbd_sysinb_cnt++;
	#endif
	*value = 0xFF & new_value;
	return returned;
}