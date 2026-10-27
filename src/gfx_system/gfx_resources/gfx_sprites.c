#include "dungeon_manager/gfx_system/gfx_resources.h"


static const uint8_t heart_img[8] = {0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00};
const sprite_t gfx_sprites[SPR_COUNT] = {
    [SPR_HEART] = { .w = 8, .h = 8, .frames = 1, .img = heart_img, .mask = NULL },
};
