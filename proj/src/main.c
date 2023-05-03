#include <lcom/lcf.h>

#include "timer.h"
#include "keyboard.h"
#include "utils.h"
#include "vg.h"
#include "i8254.h"
#include "xpm.h"

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

  if(vg_init((uint16_t) 0x105) == NULL) {
    vg_exit();
    return 1;
  }

  uint16_t gajoX = 100, gajoY = 100;
  uint8_t kbdbitno = 0, timerbitno;
  uint8_t bytes[2] = {0,0};
  if(kbd_subscribe(&kbdbitno)) return 1;
  if(timer_subscribe_int(&timerbitno)) return 1;

  kbd_read = 0x00;

  int ipc_status, driver;
  message msg;


  

  //if(print_xpm(gajoTeste, gajoX, gajoY) != 0) {
   // return 1;
  //}


  while( kbd_read != 0x81 ) { 
    
    if((driver = driver_receive(ANY,&msg,&ipc_status))!=0){ //se houver algum erro a ler
    printf("Erro a ler");
    }
    else{ //else
      if(is_ipc_notify(ipc_status)){ //se tiver recebido um interrupt da merda q quero
        switch(_ENDPOINT_P(msg.m_source)){
          case HARDWARE: //SE FOR HARDWARE INTERRUPT
          if(msg.m_notify.interrupts & kbdbitno){ //SE TIVER VINDO DO SITIO QUE EU QUERO
            kbc_ih();
            if(kbd_read == 0xe0){
                bytes[0] = 0xe0;

            }
            else{
                bool make = !(kbd_read & BIT(7));
                if(bytes[0] == 0xe0){
                    bytes[1] = kbd_read;
                    if(bytes[1] == 0x4b){ //esquerda
                      gajoX-=100;
                    }
                    else if(bytes[1] == 0x4d){ //direita
                      gajoX+=100;
                    }
                }
                else {
                    bytes[0] = kbd_read;
                    kbd_print_scancode(make, 1, bytes);
                }
                bytes[0] = 0;
                bytes[1] = 0;
            }

            
          }
          if(msg.m_notify.interrupts & timerbitno){
            timer_int_handler();
            if(timer_counter>=30){ //numero arbitrario tbh
              vg_draw_rectangle(0,0,vmi.XResolution,vmi.YResolution,0); //LIMPAR A TELA
              print_xpm(penguin,gajoX,gajoY);
              timer_counter = 0;
            }
          }
          break;
          default:
          break;
        }
      }
    }
  }
  vg_exit();
  timer_unsubscribe_int();
  return kbd_unsubscribe();
}


