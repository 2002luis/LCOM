#include <lcom/lcf.h>

#include "video_gr.h"
#include "keyboard.h"

static void *video_mem;
vbe_mode_info_t vmi;

void *(vg_init)(uint16_t mode) {
  reg86_t reg86;

  if(map_vram(mode)) {
    printf("sexo...");
    return NULL;
  }

  memset(&reg86, 0, sizeof(reg86));

  reg86.ax = 0x4f02;
  reg86.bx = (1 << 14) | mode;
  reg86.intno = 0x10;

  if(sys_int86(&reg86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }
  return video_mem;
}

int (map_vram)(uint16_t mode) {
  struct minix_mem_range mr;

  unsigned int vram_base;  /* VRAM's physical addresss */

  unsigned int vram_size;
  /* VRAM's size, but you can use
              the frame-buffer size, instead */

  unsigned int bytes_per_pixel;

  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  memset(&vmi, 0, sizeof(vmi));
  if(vbe_get_mode_info(mode, &vmi)!=OK) {
    return 1;
  }

  vram_base = vmi.PhysBasePtr;
  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;
  vram_size = (vmi.XResolution * vmi.YResolution) * bytes_per_pixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }

  return 0;
}

int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  uint8_t bytes = (vmi.BitsPerPixel + 7) / 8;
  uint8_t* base = (uint8_t*) video_mem + (y * vmi. XResolution + x) * bytes;

  for(uint8_t i = 0; i < bytes; i++){
    *base = color >> (i * 8);
    base++;
  }

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {
  for(uint16_t i = 0; i < width; i++) {
    if (vg_draw_pixel(x + i, y, color) != 0) {
      return 1;
    }
  }

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for(uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color) != 0) {
      vg_exit();
      return 1;
    }
  }

  return 0;
}
