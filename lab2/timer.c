#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t rbcommand = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  sys_outb(TIMER_CTRL, rbcommand);
  
  return util_sys_inb(TIMER(timer), st);
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  union timer_status_field_val idk;

  switch (field) {
  case tsf_all:
    idk.byte = st;
    break;

  case tsf_initial:
    uint8_t mask = BIT(5) | BIT(4);
    uint8_t thing = (st & mask) >> 4;

    switch (thing) {
      case 1:

        break;
      case 2:
        
        break;
      case 3:

        break;
    }

    conf.in_mode = 
    break;
  case tsf_mode:
    
    break;
  case tsf_base:
    
    break;
  }

  return 0;
}
