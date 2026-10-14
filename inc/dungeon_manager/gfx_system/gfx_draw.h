#pragma once

#include <stdint.h>

#include "dungeon_manager/display_system/display_manager.h"

#ifndef GFX_W
#define GFX_W 128
#define GFX_H 64
#endif


typedef enum {
    CMD_RECT, CMD_HLINE, CMD_VLINE, CMD_LINE, CMD_PATTERN,
    CMD_SPRITE, CMD_TEXT, CMD_SHAPE, CMD_COUNT
} cmd_kind_t;

typedef struct {
    uint8_t kind;                // cmd_kind_t
    uint8_t on;                  // 1 рисуем, 0 стираем
    uint8_t hidden;              // 1 = рендер пропускает
    union {
        struct { int16_t x, y; uint8_t w, h; }            rect;      // RECT, HLINE (w), VLINE (h)
        struct { int16_t x0, y0, x1, y1; }                line;
        struct { int16_t x, y; uint8_t w, h, pat; }       pattern;   // pat = display_pattern_t
        struct { int16_t x, y; uint8_t id, frame; }       sprite;
        struct { int16_t x, y; uint8_t font; const char *str; } text; // строка статическая
        struct { int16_t x, y; uint8_t id; }              shape;
    } p;
} gfx_cmd_t;


// ресурсы во флеше, данные в страничном формате SSD1306
typedef struct {
    uint8_t w, h, frames;
    const uint8_t *img;
    const uint8_t *mask;         // NULL = без маски
} sprite_t;

typedef struct {
    uint8_t w, h, first, count;  // моноширинный, глиф = w колонок * ((h + 7) / 8) страниц
    const uint8_t *data;
} font_t;

typedef struct {
    uint8_t kind;                // CMD_LINE (a,b,c,d = x0,y0,x1,y1) или CMD_RECT (a,b,c,d = x,y,w,h)
    int8_t a, b, c, d;
} shape_item_t;

typedef struct {
    uint8_t n;
    const shape_item_t *items;
} shape_t;


// gfx_resources.h (генерируется скриптом) должен дать enum с SPR_COUNT, FONT_COUNT, SHAPE_COUNT
// и таблицы: extern const sprite_t gfx_sprites[]; font_t gfx_fonts[]; shape_t gfx_shapes[];

void gfx_draw_cmd(const gfx_cmd_t *c);
