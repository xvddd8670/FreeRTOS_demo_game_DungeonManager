#pragma once

#include <stdint.h>

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_types.h"

// gfx_resources.h (генерируется скриптом) должен дать enum с SPR_COUNT, FONT_COUNT, SHAPE_COUNT
// и таблицы: extern const sprite_t gfx_sprites[]; font_t gfx_fonts[]; shape_t gfx_shapes[];

//void gfx_draw_cmd(const gfx_cmd_t *c);
void gfx_draw_gfx_obj(const gfx_obj_t *gfx_obj);
void gfx_draw_metasprite(const metasprite_t *metasprite, vector2_t coords);
void gfx_draw_one_cmd(const gfx_cmd_t *gfx_cmd, int16_t x, int16_t y);
