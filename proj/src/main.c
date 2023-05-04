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

struct player{
  int X;
  int Y;
};

struct obstacle{
  int X;
  int Y;
};

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

  bool lost = false;

  struct player p;
  struct obstacle o1;
  o1.X = 80;
  o1.Y = 0;

  p.X = 80;
  p.Y = 600;
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
  print_xpm(penguin,p.X,p.Y);

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
                if(!lost){
                if(bytes[0] == 0xe0){
                    bytes[1] = kbd_read;
                    if(bytes[1] == 0x4b && p.X>100){ //esquerda
                      vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                      p.X-=100;
                      print_xpm(penguin,p.X,p.Y);
                    }
                    else if(bytes[1] == 0x4d && p.X<880){ //direita
                      vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                      p.X+=100;
                      print_xpm(penguin,p.X,p.Y);
                    }
                  }
                  else {
                      bytes[0] = kbd_read;
                      kbd_print_scancode(make, 1, bytes);
                  }
                }
                else{

                }
                bytes[0] = 0;
                bytes[1] = 0;
            }
          }
          if(msg.m_notify.interrupts & timerbitno){
            timer_int_handler();
            if(timer_counter>=30 && !lost){ //numero arbitrario tbh
              timer_counter = 0;
              if((o1.Y+64)<vmi.YResolution) vg_draw_rectangle(o1.X,o1.Y,64,64,0);
              o1.Y += 100;
              if((o1.Y+64)<vmi.YResolution) print_xpm(pic3,o1.X,o1.Y);
              if(o1.X == p.X && o1.Y == p.Y){
                //PERDER O JOGO
                lost = true;
                vg_draw_rectangle(100,100,500,500,20);
              }
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


