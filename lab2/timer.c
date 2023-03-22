#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t cw = 0;
  timer_get_conf(timer, &cw);
  cw <<= 4;
  cw >>= 4;
  
  switch(timer) {
    case 0 : {
      cw |= TIMER_SEL0;
      break;
    } case 1 : {
      cw |= TIMER_SEL1;
      break;
    } case 2 : {
      cw |= TIMER_SEL2;
      break;
    }
  }

  cw |= TIMER_LSB_MSB;
  sys_outb(TIMER_CTRL, cw);

  uint16_t cnt = TIMER_FREQ / freq;
  uint8_t lsb = 0, msb = 0;

  util_get_LSB(cnt, &lsb);
  util_get_MSB(cnt, &msb);

  sys_outb(TIMER(timer), lsb);
  sys_outb(TIMER(timer), msb);

  return 0;
}

int hook = 0;
int count = 0;

int (timer_subscribe_int)(uint8_t *bit_no) {
  hook = *bit_no;
  
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook);
}

void (timer_int_handler)() {
  ++count;
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
    case tsf_all: {
      idk.byte = st;
      break;
    } case tsf_initial: {
      uint8_t mask = BIT(5) | BIT(4);
      uint8_t thing = (st & mask) >> 4; //evaluate the 5th and 4th bits only

      if (thing == 1) {
          idk.in_mode = LSB_only;
      } else if (thing == 2) {
        idk.in_mode = MSB_only;
      } else if (thing == 3) {
        idk.in_mode = MSB_after_LSB;
      } else {
        idk.in_mode = INVAL_val;
      } break;
    } case tsf_mode: {
      st = (st >> 1) & 0x07;

      if(st == 6) idk.count_mode = 2;
      else if(st == 7) idk.count_mode = 3;
      else idk.count_mode = st;

      break;
    } case tsf_base: {
      uint8_t mask = 1;
      st &= mask;
      idk.bcd = st;
      break;
    } default: {
      return 1;
    }
  } 
  return timer_print_config(timer, field, idk);
}
