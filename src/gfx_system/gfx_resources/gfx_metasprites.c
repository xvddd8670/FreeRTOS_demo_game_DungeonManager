#include "dungeon_manager/gfx_system/gfx_resources.h"


static const gfx_cmd_t knight_prefab[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};
const metasprite_t gfx_metasprites[METASPRITE_COUNT] = {
    [KNIGHT_MS] = {2, knight_prefab, (vector2_t){30, 30}}
};
