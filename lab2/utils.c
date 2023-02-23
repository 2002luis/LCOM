#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  return sys_inb(port, (uint32_t*) value);
}
