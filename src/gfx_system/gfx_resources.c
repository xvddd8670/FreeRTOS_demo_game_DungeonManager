#include "dungeon_manager/gfx_system/gfx_resources.h"

// сердце 8x8
static const uint8_t heart_img[8] = {0x0E, 0x1F, 0x3F, 0x7E, 0x7E, 0x3F, 0x1F, 0x0E};

// шрифт 5x8 из двух глифов: 'H' и 'I'
static const uint8_t font_small_data[2 * 5] = {
    0x7F, 0x08, 0x08, 0x08, 0x7F,    // H
    0x00, 0x41, 0x7F, 0x41, 0x00,    // I
};

// стрелка, координаты относительно (0, 0)
static const shape_item_t arrow_items[] = {
    {CMD_LINE, 0, 4, 12, 4},
    {CMD_LINE, 12, 4, 8, 0},
    {CMD_LINE, 12, 4, 8, 8},
};


const sprite_t gfx_sprites[SPR_COUNT] = {
    [SPR_HEART] = { .w = 8, .h = 8, .frames = 1, .img = heart_img, .mask = NULL },
};

const font_t gfx_fonts[FONT_COUNT] = {
    [FONT_SMALL] = { .w = 5, .h = 8, .first = 'H', .count = 2, .data = font_small_data },
};

const shape_t gfx_shapes[SHAPE_COUNT] = {
    [SHAPE_ARROW] = { .n = 3, .items = arrow_items },
};
