/*
 * display_manager.h
 *
 * implements the use of the display driver by abstracting work with it
 * allowing you to connect different displays
 *
 * you need to call the display_manager_init() in main
 */

#pragma once

#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"


#define DISPLAY_W 128
#define DISPLAY_H 64

typedef enum {
    DISPLAY_BLIT_OR,
    DISPLAY_BLIT_XOR,
    DISPLAY_BLIT_CLEAR
} display_blit_mode_t;
void display_blit(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, display_blit_mode_t mode);

typedef enum {
    DISPLAY_PAT_SOLID, DISPLAY_PAT_75, DISPLAY_PAT_50, DISPLAY_PAT_25, DISPLAY_PAT_12,
    DISPLAY_PAT_HLINES, DISPLAY_PAT_HLINES_WIDE, DISPLAY_PAT_VLINES, DISPLAY_PAT_VLINES_WIDE,
    DISPLAY_PAT_DIAG_L, DISPLAY_PAT_DIAG_R, DISPLAY_PAT_GRID, DISPLAY_PAT_COUNT
} display_pattern_t;


void need_update_set(void);
bool need_update_take(TickType_t timeout);

void display_lock(void);
void display_unlock(void);

void display_manager_init(void);
void display_init(void);
void display_clear(void);
void display_update(void);

void display_pixel(uint8_t x, uint8_t y, uint8_t pixel_on_off);
void display_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on);
void display_fill_rect_pat(uint8_t x, uint8_t y, uint8_t w, uint8_t h, display_pattern_t pat, uint8_t on);
void display_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t on);
void display_vline(uint8_t x, uint8_t y, uint8_t h, uint8_t on);
void display_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t on);
void display_blit(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, display_blit_mode_t mode);
