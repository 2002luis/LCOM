#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint16_t x_res; // horizontal resolution
  uint16_t y_res; // vertical resolution
  uint8_t bpp; // bits per pixel
  uint8_t r_sz; // red component size
  uint8_t r_pos; // red component LSB position
  uint8_t g_sz; // green component size
  uint8_t g_pos; // green component LSB position
  uint8_t b_sz; // blue component size
  uint8_t b_pos; // blue component LSB position
  phys_addr_t phys_addr; // video ram base phys address
} lpv_mode_info_t;

enum lpv_dir_t {
  lpv_hor, // horizontal line
  lpv_vert // vertical line
};

lpv_mode_info_t lmi;
void* vmem;

int lpv_map(uint16_t mode) {
  if(lpv_get_mode_info(mode, &lmi)) {
    return 1;
  }
  
  struct minix_mem_range mr;
  int r;

  unsigned int base = lmi.phys_addr;
  unsigned int bytespp = (lmi.bpp + 7) / 8;
  unsigned int size = (lmi.x_res * lmi.y_res) * bytespp;

  vmem = video_map_phys(base, size);

  if(vmem == MAP_FAILED) {
    return 1;
  }

  return 0;
}

int pp_test_line(uint8_t mode, enum lpv_dir_t dir, uint16_t x, uint16_t y, uint16_t len, uint32_t color, uint32_t delay) {~
  lpv_map(mode);

  uint8_t bytes = (lmi.bpp + 7) / 8;
  uint8_t* base = (uint8_t*) vmem + (y * lmi.x_res + x) * bytes;

  if(dir = lpv_hor) {  
    if(x + len >= lmi.x_res) len = lmi.x_res;
    for(uint16_t i = 0; i < len; i++) {
      memcpy(base, &color, bytes);
      base += bytes;
    }
  }

  if(dir = lpv_vert) {  
    if(y + len >= lmi.y_res) len = lmi.y_res;
    for(uint16_t i = 0; i < len; i++) {
      memcpy(base, &color, bytes);
      base += bytes * lmi.x_res;
    }
  }
}