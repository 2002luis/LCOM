#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int count;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st = 0;
  timer_get_conf(timer, &st);
  return timer_display_conf(timer, st, field);
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);
}

int(timer_test_int)(uint8_t time) {
  int ipcstatus = 0, driver = 0;
  uint8_t hookid = 0;
  uint32_t bit = BIT(hookid);
  message msg;

  if(timer_subscribe_int(&hookid)) return 1;
  while(time > 0) {
    if((driver = driver_receive(ANY, &msg, &ipcstatus)) != 0) {
      printf("Erro a ler");
    } else {
      if(is_ipc_notify(ipcstatus)) {
        switch(_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:
          if(msg.m_notify.interrupts & bit) {
            timer_int_handler();
            if(count % 60 == 0){
              timer_print_elapsed_time();
              time--;
            }
          }
        }
      }
    }
  }

  return timer_unsubscribe_int();
}
