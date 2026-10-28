#pragma once

#include "dungeon_manager/gfx_system/gfx_draw.h"


enum { SPR_HEART, SPR_COUNT };
enum { FONT_SMALL, FONT_COUNT };
enum { SHAPE_ARROW, SHAPE_COUNT };
enum { KNIGHT_MS, GROUND_MS, METASPRITE_COUNT };

extern const sprite_t     gfx_sprites[SPR_COUNT];
extern const font_t       gfx_fonts[FONT_COUNT];
extern const shape_t      gfx_shapes[SHAPE_COUNT];
extern const metasprite_t gfx_metasprites[METASPRITE_COUNT];
