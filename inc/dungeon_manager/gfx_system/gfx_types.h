#pragma once


#include "dungeon_manager/globals/common_types.h"

typedef enum : uint8_t {
    CMD_RECT, CMD_HLINE, CMD_VLINE, CMD_LINE, CMD_PATTERN,
    CMD_SPRITE, CMD_TEXT, CMD_SHAPE, CMD_COUNT
} cmd_type_t;

typedef struct {
    cmd_type_t type;
    uint8_t color;
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

typedef struct {
    uint8_t cmds_count;
    const gfx_cmd_t *cmds_arr;
} metasprite_t;

typedef struct {
    uint8_t id;
    const metasprite_t *metasprite;
    vector2_t coords;
} gfx_obj_t;

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
    uint8_t type;
    int8_t a, b, c, d;
} shape_item_t;

typedef struct {
    uint8_t n;
    const shape_item_t *items;
} shape_t;
