
#include "mouse.h"
#include "kbc.h"



int mouse_hook_id = 3;
struct packet mouse_packet;   // pacote gerado
uint8_t byte_index = 0;       // [0..2]
uint8_t mouse_bytes[3];       // bytes apanhados
uint8_t current_byte;         // o byte mais recente lido

int (mouse_subscribe_int)(uint8_t *bit_no){
  if (bit_no == NULL) return 1;
  *bit_no = BIT(mouse_hook_id);
  return sys_irqsetpolicy(12, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}

// Desativação das interrupções
int (mouse_unsubscribe_int)(){
  return sys_irqrmpolicy(&mouse_hook_id);
}

// Em cada chamada do IH (interrupt handler), lê um novo byte do rato
void (mouse_ih)(){
  if(read_KBC_output(0x60, &current_byte, 1)) printf("mouse ih error");
}

void mouse_sync_bytes() {
  if (byte_index == 0 && (current_byte & BIT(3))) { // é o byte CONTROL, o bit 3 está ativo
    mouse_bytes[byte_index]= current_byte;
    byte_index++;
  }
  else if (byte_index > 0) {                            // recebe os deslocamentos em X e Y
    mouse_bytes[byte_index] = current_byte;
    byte_index++;
  }
}

// Transforma o array de bytes numa struct definida de acordo com a documentação
void (mouse_bytes_to_packet)(){

  for (int i = 0 ; i < 3 ; i++) {
    mouse_packet.bytes[i] = mouse_bytes[i];
  }

  mouse_packet.lb = mouse_bytes[0] & BIT(0);
  mouse_packet.mb = mouse_bytes[0] & BIT(2);
  mouse_packet.rb = mouse_bytes[0] & BIT(1);
  mouse_packet.x_ov = mouse_bytes[0] & BIT(6);
  mouse_packet.y_ov = mouse_bytes[0] & BIT(7);
  mouse_packet.delta_x = (mouse_bytes[0] & BIT(4)) ? (0xFF00 | mouse_bytes[1]) : mouse_bytes[1];
  mouse_packet.delta_y = (mouse_bytes[0] & BIT(5)) ? (0xFF00 | mouse_bytes[2]) : mouse_bytes[2];
}

// A escrita para o rato tem de ser feita de forma mais controlada do que no keyboard
// Temos de injetar o comando DIRETAMENTE no rato e esperar por uma resposta afirmativa (ACK).
int (mouse_write)(uint8_t command) {

  uint8_t attemps = 10;
  uint8_t mouse_response;

  do {
    attemps--;
    if (write_KBC_command(0x64, 0xD4)) return 1;
    if (write_KBC_command(0x60, command)) return 1;
    tickdelay(micros_to_ticks(20000));
    if (util_sys_inb(0x60, &mouse_response)) return 1;
    if (mouse_response == 0xFA) return 0;
  } while (mouse_response != 0xFA && attemps);

  return 1;
}
