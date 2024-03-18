#include "kbc.h"

int read_status(uint8_t* stat){
  if(util_sys_inb(STATUS_REG, stat)!=0){
    return 1;
  }
  return 0;
}

int write_KBC(int port, uint8_t command){
  uint8_t stat;
  int tries=tries;
  if(util_sys_inb(STATUS_REG, &stat)!=0){
    return 1;
  }
  while(tries){
    if(read_status(stat)!=0){
      return 1;
    }
    if((command && STAT_IBF)==0){
      if(sys_out_b(STATUS_REG, command)!=0){
        return 1;
      }
      return 0;
    }
    else{
      tickdelay(micros_to_ticks(DELAY));
      tries--;
    }
  }
  return 1;
}
int read_KBC(int port, uint8_t *output){
  uint8_t stat;
  int tries=tries;
  if(read_status_code(stat)!=0){
      return 1;
  }
  while(tries){
    if((stat & STAT_OBF)!=0){
      if((stat & STAT_PARITY)!=0 && (stat & STAT_TIMEOUT)!=0){
        return 1;
      }

      if(util_sys_inb(port, &output)!=0){
        return 1;
      }
      return 0;
    }
    else{
      tickdelay(micros_to_ticks(DELAY));
      tries--;
    }
    return 1;
  }
}