#include "dungeon_manager/gfx_system/gfx_resources.h"

static const shape_item_t arrow_items[] = {
    {CMD_LINE, 0, 4, 12, 4},
    {CMD_LINE, 12, 4, 8, 0},
    {CMD_LINE, 12, 4, 8, 8},
};
const shape_t gfx_shapes[SHAPE_COUNT] = {
    [SHAPE_ARROW] = { .n = 3, .items = arrow_items },
};


