/*
 * ssd1306.h
 *
 * this is the ssd1306 i2c OLED display driver
 * it works over i2c via DMA
 *
 * it must be connected only through the display manager
 * otherwise incorrect operation is possible
 * if you take it into your project, you can skip this point
 *
 */

#pragma once
#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Pixel(uint8_t x, uint8_t y, uint8_t on);
void OLED_Update(void);
