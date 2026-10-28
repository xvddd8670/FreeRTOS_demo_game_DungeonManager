#include "dungeon_manager/gfx_system/gfx_resources.h"


static const gfx_cmd_t knight_prefab[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};

static const gfx_cmd_t ground_prefab[] = {
    { .type = CMD_PATTERN, .color = 1, .payload.pattern = { .x=(-8), .y=(-8), .w=16, .h=16, .pat=2} },
    // { .type = CMD_RECT, .color = 1, .payload.pattern = { .x=(-8), .y=(-8), .w=16, .h=16} },
    { .type = CMD_LINE,   .color = 1, .payload.line   = { -8, -8, 7, -8} },          // линия вправо от pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { -8, -8, -8, 7} },          // линия вправо от pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 8, 8, -8, 8} },          // линия вправо от pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 8, 8, 8, -8} },          // линия вправо от pivot
};

const metasprite_t gfx_metasprites[METASPRITE_COUNT] = {
    [KNIGHT_MS] = {.cmds_count = 2, .cmds_arr = knight_prefab},
    [GROUND_MS] = {.cmds_count = 5, .cmds_arr = ground_prefab},
};
