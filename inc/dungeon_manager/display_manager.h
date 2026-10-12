#pragma once
#include <stdint.h>

void display_init(void);
void display_clear(void);
void display_pixel(uint8_t x, uint8_t y, uint8_t pixel_on_off);
void display_update(void);

