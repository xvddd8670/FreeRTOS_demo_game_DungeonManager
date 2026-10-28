#pragma once

#include <stdint.h>

#include "dungeon_manager/globals/common_types.h"

#define GAME_GFX_ID 0

#define MAX_ENTITIES 128
#define MASK_EMPTY 0
#define FLOOR_W 11
#define FLOOR_H 11

typedef enum {
    cb_alive            = 1 << 0,
    cb_ground           = 1 << 1,
    cb_corridor         = 1 << 2,
    cb_generator        = 1 << 3,
    cb_processor        = 1 << 4,
} component_bits_t;

// -----------------------------
// ecs objects
// -----------------------------

extern uint8_t current_floor[FLOOR_W][FLOOR_H];
extern uint16_t entities_masks[MAX_ENTITIES];

typedef struct{
    uint8_t ground_type;
} ground_component;
extern ground_component ground_components[MAX_ENTITIES];

typedef struct{
} corridor_component;
extern corridor_component corridor_components[MAX_ENTITIES];

typedef struct{
    uint8_t power_production;
} generator_component;
extern generator_component generator_components[MAX_ENTITIES];

typedef struct{
    uint8_t pflops;
    uint8_t power_consumption;
} processor_component;
extern processor_component processor_components[MAX_ENTITIES];

typedef struct{
} elevator_component;
extern elevator_component elevator_components[MAX_ENTITIES];

typedef struct{
    vector2_t coord;
    uint8_t floor;
    int8_t gfx_obj_id;
} tile_component;
extern tile_component tile_components[MAX_ENTITIES];

// ------------------------------
// ecs systems
// ------------------------------
void calculate_base_production(void);

// ------------------------------
// functions
// ------------------------------
void game_init(void);
