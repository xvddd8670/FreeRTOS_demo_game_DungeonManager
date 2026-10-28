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

#define OLED_W 128
#define OLED_H 64

enum {
    OLED_PAT_SOLID, OLED_PAT_75, OLED_PAT_50, OLED_PAT_25, OLED_PAT_12,
    OLED_PAT_HLINES, OLED_PAT_HLINES_WIDE, OLED_PAT_VLINES, OLED_PAT_VLINES_WIDE,
    OLED_PAT_DIAG_L, OLED_PAT_DIAG_R, OLED_PAT_GRID, OLED_PAT_COUNT
};

extern const uint8_t oled_pat[OLED_PAT_COUNT][8];

void oled_init(void);
void oled_clear(void);
void oled_update(void);
//void OLED_Snapshot(void);
//void OLED_Send(void);

void oled_pixel(uint8_t x, uint8_t y, uint8_t on);
void oled_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t on);
void oled_fill_rect_pat(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *pat, uint8_t on);
void oled_hline(int16_t x, int16_t y, int16_t w, uint8_t on);
void oled_vline(int16_t x, int16_t y, int16_t h, uint8_t on);
void oled_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t on);
void oled_blit_mono(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, uint8_t on);
