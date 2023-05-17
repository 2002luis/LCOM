#include <lcom/lcf.h>

#include "mouse.h"
#include "timer.h"
#include "keyboard.h"
#include "utils.h"
#include "vg.h"
#include "i8254.h"
#include "xpm2.h"
#include "rtc.h"


#include <stdint.h>

#include "i8254.h"

extern int timer_hook_id;
extern int kbd_hook_id;
extern int timer_counter;
extern uint8_t kbd_read;
extern void *video_mem;
extern vbe_mode_info_t vmi;
extern struct packet pckt;
extern uint8_t bIndex;

struct pointer{
  int X;
  int Y;
};

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
  int speed;
  xpm_row_t* img;
  bool active;
};

int timeUntilSpeedUp = 50, maxTime = 50;
float speedMul = 1;

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

int (clamp)(int a, int mn, int mx){
  if(a<mn) return mn;
  if(a>mx) return mx;
  return a;
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

  struct pointer p2;

  p2.X = 500;
  p2.Y = 150;

  bool day = true;
  uint8_t hour = 0;
  while(rtcReadHours(&hour));
  day = (hour>=7 && hour<=18);

  bool lost = false;
  bool tempIgnoreLeftMouse = false, tempIgnoreRightMouse = false;

  struct player p;
  struct obstacle o[5];
  int nObs = 5;

  o[0].img = pic3;

  o[0].X = 100;
  o[0].Y = 0;
  o[0].XLen = 64;
  o[0].YLen = 32;
  o[0].active = true;
  o[0].speed = 20;

  for(int i = 0; i < nObs; i++) o[i].active = false;

  p.img = penguin;

  p.X = 100;
  p.Y = 600;
  p.XLen = 64;
  p.YLen = 64;
  uint8_t kbdbitno = 0, timerbitno, mousebitno;
  uint8_t bytes[2] = {0,0};
  if(kbd_subscribe(&kbdbitno)) return 1;
  if(timer_subscribe_int(&timerbitno)) return 1;
  if (mouse_subscribe(&mousebitno) != 0) return 1;
  if (mouse_write(0xf4) != 0) return 1;
  kbd_read = 0x00;

  int ipc_status, driver;
  message msg;

  srand(time(NULL));
  allocateBuffer();
  bool endgame = false;
  //if(print_xpm(gajoTeste, gajoX, gajoY) != 0) {
   // return 1;
  //}
  bool left = false, right = false, tempDisableLeftKbd = false, tempDisableRightKbd = false;
  print_xpm(penguin,p.X,p.Y);
  while( !endgame ) { 
    
    if((driver = driver_receive(ANY,&msg,&ipc_status))!=0){ //se houver algum erro a ler
    printf("Erro a ler");
    }
    else{ //else
      if(is_ipc_notify(ipc_status)){ //se tiver recebido um interrupt da merda q quero
        switch(_ENDPOINT_P(msg.m_source)){
          case HARDWARE: //SE FOR HARDWARE INTERRUPT
          if(msg.m_notify.interrupts & kbdbitno){ //SE TIVER VINDO DO SITIO QUE EU QUERO
            kbc_ih();
            if(kbd_read == 0x81){
              mouse_write(0xf5);
              endgame = true;
            }
            else if(kbd_read == 0xe0){
                bytes[0] = 0xe0;

            }
            else{
                //bool make = !(kbd_read & BIT(7));
                if(!lost){
                if(bytes[0] == 0xe0){
                    bytes[1] = kbd_read;

                    if(bytes[1] == 0x4b) left = true;
                    else if(bytes[1] == 0xcb) 
                    {
                      tempDisableLeftKbd = false;
                      left = false;
                    }
                    else if(bytes[1] == 0x4d) right = true;
                    else if(bytes[1] == 0xcd) 
                    {
                      tempDisableRightKbd = false;
                      right = false;
                    }
                    /*else if(bytes[1] == 0x48) up = true;
                    else if(bytes[1] == 0xc8) up = false;
                    else if(bytes[1] == 0x50) down = true;
                    else if(bytes[1] == 0xd0) down = false;*/   

                    
                  }
                  else {
                      bytes[0] = kbd_read;
                      //kbd_print_scancode(make, 1, bytes);
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
              timeUntilSpeedUp--;
              if(timeUntilSpeedUp == 0){
                speedMul += 0.1;
                timeUntilSpeedUp = maxTime;
              }
              clearBuffer();
              while(rtcReadHours(&hour));
              day = (hour>=7 && hour<=18);

              if(day) print_xpm(sun,50,50);
              else print_xpm(moon,50,50);
              if(left && !right && p.X>100 && !tempDisableLeftKbd){ //esquerda
                //vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                p.X-=200;
                tempDisableLeftKbd = true;
                //print_xpm(p.img,p.X,p.Y);
              }
              else if(right && !left && p.X<890 && !tempDisableRightKbd){ //direita
                //vg_draw_rectangle(p.X,p.Y,64,64,0); //LIMPAR A TELA
                p.X+=200;
                tempDisableRightKbd = true;
                //print_xpm(p.img,p.X,p.Y);
              }

              timer_counter = 0;
              //if((o[0].Y+64)<vmi.YResolution) vg_draw_rectangle(o[0].X,o[0].Y,64,64,0);
              if (!lost) for(int i = 0; i < nObs; i++){
                if(o[i].active){
                  o[i].Y += o[i].speed*speedMul;
                  if((o[i].Y+o[i].YLen)>vmi.YResolution) o[i].active = false;
                  if(o[i].active) print_xpm(o[i].img,o[i].X,o[i].Y);
                  if(intersects(p,o[i])){
                    //print_xpm(p.img,p.X,p.Y);
                    //print_xpm(pic3,o[0].X,o[0].Y);
                    //PERDER O JOGO
                    lost = true;
                  }
                }
              }
              if(lost) vg_draw_rectangle(100,100,500,500,20);
              vg_draw_rectangle(p2.X,p2.Y,20,20,5);
              print_xpm(p.img,p.X,p.Y);
              showBuffer();
            }
          }
          if (msg.m_notify.interrupts & mousebitno){
            mouse_ih();
            readBytes();
            if (bIndex == 3) {
              //vg_draw_rectangle(p2.X,p2.Y,20,20,0);
              toPacket();
              p2.X+=(pckt.delta_x)/1;
              //p2.Y-=(pckt.delta_y)/50;
              p2.X = clamp(p2.X,120,880);
              //p2.Y = clamp(p2.Y, 10, 300);
              //vg_draw_rectangle(p2.X,p2.Y,20,20,5);
              if(pckt.lb && !tempIgnoreLeftMouse){
                for(int i = 0; i < nObs; i++)
                if(!o[i].active){
                  o[i].X = (p2.X/200)*200+100;
                  o[i].Y = p2.Y;
                  o[i].XLen = 64;
                  o[i].YLen = 32;
                  o[i].active = true;
                  o[i].img = pic3;
                  o[i].speed = 20;
                  tempIgnoreLeftMouse = true;
                  break;
                }
              }
              else if(pckt.rb && !tempIgnoreRightMouse){
                for(int i = 0; i < nObs; i++) if(!o[i].active){
                  o[i].X = (p2.X/200)*200+100;
                  o[i].Y = p2.Y;
                  o[i].XLen = 100;
                  o[i].YLen = 100;
                  o[i].active = true;
                  o[i].img = gajoTeste;
                  o[i].speed = 15;
                  tempIgnoreRightMouse = true;
                  break;
                }
              }
              if(!pckt.lb) tempIgnoreLeftMouse = false; //evitar repetidos
              if(!pckt.rb) tempIgnoreRightMouse = false;
              bIndex = 0;
              if(kbd_read == 0x81)
              {
                mouse_write(0xf5);
                endgame = true;
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
  freeBuffer();
  vg_exit();
  timer_unsubscribe_int();
  mouse_unsubscribe();
  return kbd_unsubscribe();
}


