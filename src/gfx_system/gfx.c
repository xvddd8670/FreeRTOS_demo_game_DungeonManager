#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_task.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"

render_single_queue_map render_queue[QUEUES_OWNER_COUNT];


static uint32_t ids_pool[8];

int16_t get_free_id(void){
    for (uint8_t w = 0; w < 8; w++){
        if (ids_pool[w] != 0xFFFFFFFF){
            uint8_t bit = __builtin_ctz(~ids_pool[w]);
            ids_pool[w] |= (uint32_t)1 << bit;
            return w * 32 + bit;
        }
    }
    return -1;
}

void id_free(uint8_t id){
    ids_pool[id / 32] &= ~((uint32_t)1 << (id % 32));
}

uint16_t add_to_render_queue(QueueOwner owner_id, gfx_obj_t gfx_obj){
    uint16_t gfx_obj_id = get_free_id();
    if (render_single_queue_map_put(&render_queue[owner_id], gfx_obj_id, gfx_obj) == false){
        return -1;
    } else {
        return gfx_obj_id;
    }
}

bool remove_from_render_queue(QueueOwner owner_id, uint16_t gfx_obj_id){
    return render_single_queue_map_remove(&render_queue[owner_id], gfx_obj_id);
}

bool move_obj_coord_on_render_queue(QueueOwner owner_id, uint16_t gfx_obj_id, vector2_t vector_to_move){
    gfx_obj_t *gfx_obj_to_move = render_single_queue_map_get(&render_queue[owner_id], gfx_obj_id);
    if (gfx_obj_to_move != NULL){
        gfx_obj_to_move->coords.x += vector_to_move.x;
        gfx_obj_to_move->coords.y += vector_to_move.y;
        return true;
    } else {
        return false;
    }
}

static const gfx_cmd_t knight_cmds[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};
static const metasprite_t knight = {2, knight_cmds, (vector2_t){30, 30}};

static const gfx_cmd_t text[] = {
    { .type = CMD_TEXT,    .color = 1, .payload.text    = {0, 0, FONT_SMALL, "HELLO WORLD"} },
};
static const metasprite_t text_ms = { .cmds_count = 1, .cmds_arr = text, };

gfx_obj_t knight_gfx_obj = {0, &knight, (vector2_t){0, 0}};
gfx_obj_t text_gfx_obj = {1, &text_ms, (vector2_t){30, 0}};

void gfx_system_init(){
    add_to_render_queue(0, knight_gfx_obj);
    add_to_render_queue(0, text_gfx_obj);
    xTaskCreate(gfx_task, "gfx", 512, NULL, 1, NULL);
}


