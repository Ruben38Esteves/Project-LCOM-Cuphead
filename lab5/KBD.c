#include "KBD.h"
#include "KBC.c"
int kbd_hook_id=1;
uint8_t scancode;
int(kbd_subscribe_int)(uint8_t* bit_no){
  if(bit_no==NULL)
    return 1;
  *bit_no=BIT(kbd_hook_id);
  if(sys_irqsetpolicy(IRQ_KBD,IRQ_REENABLE |IRQ_EXCLUSIVE, &kbd_hook_id)!=0)
    return 1;
  return 0;
}

int (kbd_unsubscribe_int)(){
  if(sys_irqrmpolicy(&kbd_hook_id)!=0)
    return 1;
  return 0;
}

void (kbc_ih)(){
  KBC_read_output(0x60,&scancode, 0);
}



//ESCREVER PARA O CONTROLADOR(0X64) QUE VAI LER (0X20) 
//IR BUSCAR O CODIGO AO 0X60
//ATIVAR O BIT 0 (BIT DE ENABLE DAS INTERRUPÇÕES DO TECLADO)
//ESCREVER NO CONTROLADOR (0X64) QUE VAI ESCREVER (0X60)
//ESCREVE NO 0X60 O CODIGO QUE FOI BUSCAR
int (keyboard_restore)(){
    uint8_t comando;
    if(KBC_write_comm(0x64,0x20)!=0)
      return 1;
    if(KBC_read_output(0x60,&comando, 0)!=0)
      return 1;
    comando |= BIT(0);
    if(KBC_write_comm(0x64,0x60)!=0)
      return 1;
    if(KBC_write_comm( 0X60,comando)!=0)
      return 1;
    return 0;
}

































/* int kbd_hook_id=1;
uint8_t scancode=0;


int(kbd_subscribe_int)(uint8_t* bit_no){
  if(bit_no==NULL)
    return 1;
  *bit_no=BIT(kbd_hook_id);
  if(sys_irqsetpolicy(IRQ_KBD,IRQ_REENABLE|IRQ_EXCLUSIVE,&kbd_hook_id)!=0)
    return 1;
  return 0;
}
int (kbd_unsubscribe_int)(){
  if(sys_irqrmpolicy(&kbd_hook_id)!=0)
    return 1;
  return 0;
}
void (kbc_ih)(){
  KBC_read_output(0x60,&scancode,0);
}
//ESCREVER PARA O CONTROLADOR(0X64) QUE VAI LER (0X20) 
//IR BUSCAR O CODIGO AO 0X60
//ATIVAR O BIT 0 (BIT DE ENABLE DAS INTERRUPÇÕES DO TECLADO)
//ESCREVER NO CONTROLADOR (0X64) QUE VAI ESCREVER (0X60)
//ESCREVE NO 0X60 O CODIGO QUE FOI BUSCAR
int (keyboard_restore)(){
  uint8_t command;
  if(KBC_write_comm(0x20,0x64)!=0)
    return 1;
  if(KBC_read_output(0x60,&command, 0)!=0)
    return 1;
  command =command | BIT(0);
  if(KBC_write_comm(0x60,0x64)!=0)
    return 1;
  if(KBC_write_comm(command,0x60)!=0)
    return 1;
  return 0;
}
 */

/* void(kbc_ih)(){
  if (KBC_read_output(KBD_DATA_BUF,&scancode,0)!=0) printf("Error: Could not read scancode!\n");
}
int(kbd_subscribe_int)(uint8_t* bit_no) {
  if(bit_no==NULL)
    return 1;
  *bit_no=BIT(kbd_hook_id);
  if(sys_irqsetpolicy(IRQ_KBD,IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id)!=0)
    return 1;
  return 0;
}
int (kbd_unsubscribe_int)(){
  if(sys_irqrmpolicy(&kbd_hook_id)!=0)
    return 1;
  return 0;
}
int (keyboard_restore)(){
  uint8_t command_byte;
  if(KBC_write_comm(KB_FE,KBD_CMD_BUF)!=0)
    return 1;
  if(KBC_read_output(KBD_OUT_BUF,&command_byte,0)!=0)
    return 1;
  command_byte= command_byte | BIT(0);
  if(KBC_write_comm(KBD_OUT_BUF,KBD_CMD_BUF)!=0)
    return 1;
  if(KBC_write_comm(command_byte,KBD_OUT_BUF)!=0)
    return 1;
  return  0;
}*/

























// int kbd_hook_id=1;
// uint8_t scancode=0;
// int(kbd_subscribe_int)(uint8_t* bit_no){
//   if(bit_no==NULL) return 1;
//   *bit_no=BIT(kbd_hook_id); //guardo hoodId antes de o alterar
//   if(sys_irqsetpolicy(IRQ_KBD, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id)==0)return 0;
//   return 1;
// }
// int (kbd_unsubscribe_int)(){
//   return sys_irqrmpolicy(&kbd_hook_id);
// }
// void (kbc_ih)() {
//     if (KBC_read_output(KBD_DATA_BUF,&scancode,0)!=0) printf("Error: Could not read scancode!\n");
// }

// int (keyboard_restore)(){
//   uint8_t comm;
//   if(KBC_write_comm(KB_FE,KBD_CMD_BUF)!=0)
//     return 1;
//   if(KBC_read_output(KBD_OUT_BUF,&comm,0)!=0)
//     return 1;
//   comm= comm | BIT(0);
//   if(KBC_write_comm(KBD_OUT_BUF,KBD_CMD_BUF)!=0)
//     return 1;
//   if(KBC_write_comm(comm,KBD_OUT_BUF)!=0)
//     return 1;
//   return 0;
// }


