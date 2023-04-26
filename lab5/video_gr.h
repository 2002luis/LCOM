#pragma once
 
#include <stdint.h>
 
void *(vg_init)(uint16_t mode);

int (map_vram)(uint16_t mode);
 
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
 
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
 
int (vg_display_vbe_contr_info)(struct vg_vbe_contr_info *info_p);

int (kbd_loop)();
