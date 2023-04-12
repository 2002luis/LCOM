#include <lcom/lcf.h>

#include "video_gr.h"

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

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  /* mapping */
  
  vbe_mode_info_t vmi;

  vbe_get_mode_info(mode, &vmi);

  static void *video_mem;

  struct minix_mem_range mr;
  unsigned int vram_base = vmi.PhysBasePtr; /* VRAM's physical addresss */
  unsigned int vram_size = (vmi.XResolution * vmi.YResolution) * (vmi.BytesPerScanLine / vmi. XResolution);  /* VRAM's size, but you can use
              the frame-buffer size, instead */
  int r;

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if(OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");

}

