#include <lcom/lcf.h>

#include "video_gr.h"

static void *video_mem;
static int vram_size;
static int bytes_per_pixel;
vbe_mode_info_t vmi;

void *(vg_init)(uint16_t mode) {
  reg86_t reg86;

  memset(&reg86, 0, sizeof(reg86));

  reg86.ax = 0x4f02;
  reg86.bx = (1 << 14) | mode;
  reg86.intno = 0x10;

  if(sys_int86(&reg86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }
  return NULL;
}

int (map_vram)(uint16_t mode) {
  struct minix_mem_range mr;

  unsigned int vram_base;  /* VRAM's physical addresss */

  //unsigned int vram_size;
  /* VRAM's size, but you can use
              the frame-buffer size, instead */
  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  if(vbe_get_mode_info(mode,&vmi)!=OK) {
    return 1;
  }

  vram_base = vmi.PhysBasePtr;
  bytes_per_pixel = vmi.BytesPerScanLine / vmi.XResolution;
  vram_size = (vmi.XResolution * vmi.YResolution) * bytes_per_pixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size*3;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
  }

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  memset(video_mem, 0, vram_size);

  if(video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }

  return 0;
}

void(draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int i = 0; i < len; i++) {
    draw_pixel(x, y, color);
    x++;
  }

  return EXIT_SUCCESS;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (y + height > vmi.YResolution)
    height = vmi.YResolution - y;
  
  if (x + width > vmi.XResolution)
    width = vmi.XResolution - x;

  for (int i = 0; i < height; i++) {
    vg_draw_hline(x, y, width, color);
    y++;
  }
  return EXIT_SUCCESS;
}
