#pragma once

#include <stdint.h>

#include "dungeon_manager/display_system/display_manager.h"

typedef enum {
    CMD_RECT, CMD_HLINE, CMD_VLINE, CMD_LINE, CMD_PATTERN,
    CMD_SPRITE, CMD_TEXT, CMD_SHAPE, CMD_COUNT
} cmd_type_t;

typedef struct {
    uint8_t type;                // cmd_kind_t
    uint8_t color;               // color_t, 0 = фон (стирание)
    uint8_t hidden;
    union {
        struct { int16_t x, y, w, h; }                    rect;
        struct { int16_t x0, y0, x1, y1; }                line;
        struct { int16_t x, y, w, h; uint8_t pat; }       pattern;
        struct { int16_t x, y; uint8_t id, frame; }       sprite;
        struct { int16_t x, y; uint8_t font; const char *str; } text;
        struct { int16_t x, y; uint8_t id; }              shape;
    } payload;
} gfx_cmd_t;

// ресурсы во флеше, битмапы 1bpp: по строкам, MSB слева, строка выровнена до байта
typedef struct {
    uint8_t w, h, frames;
    const uint8_t *img;
    const uint8_t *mask;
} sprite_t;

typedef struct {
    uint8_t w, h, first, count;  // моноширинный, глиф = w колонок * ((h + 7) / 8) страниц
    const uint8_t *data;
} font_t;

typedef struct {
    uint8_t type;                // CMD_LINE (a,b,c,d = x0,y0,x1,y1) или CMD_RECT (a,b,c,d = x,y,w,h)
    int8_t a, b, c, d;
} shape_item_t;

typedef struct {
    uint8_t n;
    const shape_item_t *items;
} shape_t;


// gfx_resources.h (генерируется скриптом) должен дать enum с SPR_COUNT, FONT_COUNT, SHAPE_COUNT
// и таблицы: extern const sprite_t gfx_sprites[]; font_t gfx_fonts[]; shape_t gfx_shapes[];

void gfx_draw_cmd(const gfx_cmd_t *c);
