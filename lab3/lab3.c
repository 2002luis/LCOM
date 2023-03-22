#include <lcom/lcf.h>
#include "lab3.h"
#include "keyboard.h"

#define KBD_IRQ 1
#define KBD_OUT_BUF 0x60
#define KBD_STAT_REG 0x64



#define DELAY_US    20000

extern int hook_id;
extern uint8_t kbd_read;
extern int sys_inb_count;

int (kbd_test_poll)(){
  
  
  uint8_t bytes[2] = {0};
  bool make;

  while(kbd_read != 0x81){

    if(kbd_output_buffer_reader(&kbd_read) == 0){
      if(kbd_read == 0xe0){
        bytes[0] = 0xe0;
      }

      else{
          make = !(kbd_read & BIT(7));
          if(bytes[0] == 0xe0){
              bytes[1] = kbd_read;
              kbd_print_scancode(make, 2, bytes);
          }
          else {
              bytes[0] = kbd_read;
              kbd_print_scancode(make, 1, bytes);
          }
          bytes[0] = 0;
          bytes[1] = 0;
      }
    }
  
}

  kbd_print_no_sysinb(sys_inb_count);
  return 0;
}

int (kbd_test_timed_scan)(uint8_t idle){
  return 1;
}

int (kbd_test_scan)(){
  int ipcstatus, driver;
  uint8_t coco = 2;
  uint32_t hookid = BIT(coco);
  message msg;
  uint8_t bytes[2] = {0};
  bool make;

  kbd_subscribe(&coco);
  while(kbd_read != 0x81){

  if((driver = driver_receive(ANY,&msg,&ipcstatus))!=0){ //se houver algum erro a ler
    printf("Erro a ler");
  }
  else{ //else
    if(is_ipc_notify(ipcstatus)){ //se tiver recebido um interrupt da merda q quero
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE: //SE FOR HARDWARE INTERRUPT
        if(msg.m_notify.interrupts & hookid){ //SE TIVER VINDO DO SITIO QUE EU QUERO
          kbc_ih();
          if(kbd_read == 0xe0){
              bytes[0] = 0xe0;

          }
          else{
              make = !(kbd_read & BIT(7));
              if(bytes[0] == 0xe0){
                  bytes[1] = kbd_read;
                  kbd_print_scancode(make, 2, bytes);
              }
              else {
                  bytes[0] = kbd_read;
                  kbd_print_scancode(make, 1, bytes);
              }
              bytes[0] = 0;
              bytes[1] = 0;
          }
        }
        break;
        default:
        break;
      }
    }
  }
}

  kbd_print_no_sysinb(sys_inb_count);

  return kbd_unsubscribe();
}



int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
