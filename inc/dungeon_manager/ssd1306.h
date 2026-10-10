#pragma once
#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Pixel(uint8_t x, uint8_t y, uint8_t on);
void OLED_Update(void);
