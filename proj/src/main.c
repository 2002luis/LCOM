#include <lcom/lcf.h>

#include "timer.h"
#include "keyboard.h"
#include "utils.h"
#include "vg.h"
#include "i8254.h"

#include <stdint.h>

#include "i8254.h"

extern int timer_hook_id;
extern int kbd_hook_id;
extern int timer_counter;
extern uint8_t kbd_read;
extern void *video_mem;
extern vbe_mode_info_t vmi;

int main(int argc, char *argv[]){

  lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

    return 0;
}

int (proj_main_loop)(){

  int ipcstatus, driver;
  uint8_t coco = 2;
  uint32_t hookid = BIT(coco);
  message msg;
  uint8_t bytes[2] = {0};
  bool make;

  kbd_read = 0x00;

  kbd_subscribe(&coco);
  while(kbd_read != 0x81) {

  if((driver = driver_receive(ANY,&msg,&ipcstatus))!=0) { //se houver algum erro a ler
    printf("Erro a ler");
  } else { //else
    if(is_ipc_notify(ipcstatus)) { //se tiver recebido um interrupt da merda q quero
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //SE FOR HARDWARE INTERRUPT
        if(msg.m_notify.interrupts & hookid) { //SE TIVER VINDO DO SITIO QUE EU QUERO
          kbc_ih();
          if(kbd_read == 0xe0) {
              bytes[0] = 0xe0;

          } else {
              make = !(kbd_read & BIT(7));
              if(bytes[0] == 0xe0) {
                  bytes[1] = kbd_read;
                  //kbd_print_scancode(make, 2, bytes);
              } else {
                  bytes[0] = kbd_read;
                  //kbd_print_scancode(make, 1, bytes);
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

  //kbd_print_no_sysinb(sys_inb_count);

  return kbd_unsubscribe();
}
