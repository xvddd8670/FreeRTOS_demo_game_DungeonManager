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


#define DISPLAY_COLOR_BG   0      // фон, "стереть"
#define DISPLAY_COLOR_FG   1      // основной цвет (на монохроме светится)
#define DISPLAY_COLOR_NONE 255    // резерв: прозрачный пиксель

typedef enum {
    DISPLAY_PAT_SOLID, DISPLAY_PAT_75, DISPLAY_PAT_50, DISPLAY_PAT_25, DISPLAY_PAT_12,
    DISPLAY_PAT_HLINES, DISPLAY_PAT_HLINES_WIDE, DISPLAY_PAT_VLINES, DISPLAY_PAT_VLINES_WIDE,
    DISPLAY_PAT_DIAG_L, DISPLAY_PAT_DIAG_R, DISPLAY_PAT_GRID, DISPLAY_PAT_COUNT
} display_pattern_t;

typedef uint8_t color_t;

uint16_t display_width(void);
uint16_t display_height(void);

void need_update_set(void);
bool need_update_take(TickType_t timeout);

void display_lock(void);
void display_unlock(void);

void display_manager_init(void);
void display_init(void);
void display_clear(void);
void display_update(void);

void display_pixel(int16_t x, int16_t y, color_t c);
void display_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, color_t c);
void display_fill_rect_pat(int16_t x, int16_t y, int16_t w, int16_t h, display_pattern_t pat, color_t c);
void display_hline(int16_t x, int16_t y, int16_t w, color_t c);
void display_vline(int16_t x, int16_t y, int16_t h, color_t c);
void display_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t c);
void display_blit_mono(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, color_t c);
