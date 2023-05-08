#include <lcom/lcf.h>

#include "timer.h"
#include "keyboard.h"
#include "utils.h"
#include "vg.h"
#include "i8254.h"
#include "xpm2.h"

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
  int XLen, YLen;
  xpm_row_t* img;
};

struct obstacle{
  int X;
  int Y;
  int XLen, YLen;
  xpm_row_t* img;
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

int (max)(int a, int b){
  if(a>b) return a;
  return b;
}

int (min)(int a, int b){
  if(a<b) return a;
  return b;
}

bool (intersects)(struct player p, struct obstacle o){
  if((p.X+p.XLen-20) < o.X || (p.Y+p.YLen-20) < o.Y || p.Y > (o.Y+o.YLen-20) || p.X > (o.X+o.XLen-20)) return false;
  return true;
}

int (proj_main_loop)(){

  if(vg_init((uint16_t) 0x105) == NULL) {
    vg_exit();
    return 1;
  }

  bool lost = false;

  struct player p;
  struct obstacle o1;

  o1.img = pic3;

  o1.X = 100;
  o1.Y = 0;
  o1.XLen = 64;
  o1.YLen = 32;


  p.img = penguin;

  p.X = 100;
  p.Y = 600;
  p.XLen = 64;
  p.YLen = 64;
  uint8_t kbdbitno = 0, timerbitno;
  uint8_t bytes[2] = {0,0};
  if(kbd_subscribe(&kbdbitno)) return 1;
  if(timer_subscribe_int(&timerbitno)) return 1;

  kbd_read = 0x00;

  int ipc_status, driver;
  message msg;

  srand(time(NULL));
  

  //if(print_xpm(gajoTeste, gajoX, gajoY) != 0) {
   // return 1;
  //}
  bool left = false, right = false, up = false, down = false;
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

                    if(bytes[1] == 0x4b) left = true;
                    else if(bytes[1] == 0xcb) left = false;
                    else if(bytes[1] == 0x4d) right = true;
                    else if(bytes[1] == 0xcd) right = false;
                    else if(bytes[1] == 0x48) up = true;
                    else if(bytes[1] == 0xc8) up = false;
                    else if(bytes[1] == 0x50) down = true;
                    else if(bytes[1] == 0xd0) down = false;

                    
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
            if(timer_counter>=5 && !lost){ //numero arbitrario tbh

              if(left && !right && p.X>100){ //esquerda
                vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                p.X-=20;
                print_xpm(p.img,p.X,p.Y);
              }
              else if(right && !left && p.X<890){ //direita
                vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                p.X+=20;
                print_xpm(p.img,p.X,p.Y);
              }
              else if(up && !down && p.Y > 200){ //cima
                vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                p.Y-=20;
                print_xpm(p.img,p.X,p.Y);
              }
              else if(down && !up && p.Y < 600){ //cima
                vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                p.Y+=20;
                print_xpm(p.img,p.X,p.Y);
              }


              timer_counter = 0;
              if((o1.Y+64)<vmi.YResolution) vg_draw_rectangle(o1.X,o1.Y,64,64,0);
              o1.Y += 20;
              if((o1.Y+64)<vmi.YResolution) print_xpm(pic3,o1.X,o1.Y);
              else{
                o1.Y = 0;
                o1.X = max((rand()%800)+100,(900-o1.XLen));
              }
              if(intersects(p,o1)){
                print_xpm(p.img,p.X,p.Y);
                print_xpm(pic3,o1.X,o1.Y);
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


