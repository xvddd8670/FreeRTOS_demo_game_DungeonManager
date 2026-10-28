#include "dungeon_manager/utils/log.h"

#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_task.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"

render_single_queue_map render_queue[QUEUES_OWNER_COUNT];
xSemaphoreHandle render_queue_mutex;

static uint32_t ids_pool[8];// = {1};

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
    // if (id == 0) return;
    ids_pool[id / 32] &= ~((uint32_t)1 << (id % 32));
}

void gfx_render_queue_lock(void) {
    xSemaphoreTakeRecursive(render_queue_mutex, portMAX_DELAY);
}
void gfx_render_queue_unlock(void){
    xSemaphoreGiveRecursive(render_queue_mutex);
}

int16_t gfx_add_to_render_queue(QueueOwner owner_id, gfx_obj_t gfx_obj){
    int16_t gfx_obj_id = get_free_id();
    if (gfx_obj_id == -1){
        return -1;
    }
    gfx_obj.id = gfx_obj_id;
    gfx_render_queue_lock();
    bool ok = render_single_queue_map_put(&render_queue[owner_id], gfx_obj_id, gfx_obj);
    gfx_render_queue_unlock();
    if (!ok){
        LOG_ERROR("error add to render queue");
        id_free(gfx_obj_id);
        return -1;
    }
    return gfx_obj_id;
}

bool gfx_remove_from_render_queue(QueueOwner owner_id, uint16_t gfx_obj_id){
    // LOG_DEBUG("remove from render queue");
    // LOG_DEBUG("count before: %d", render_queue[owner_id].len);

    bool remove_succes = render_single_queue_map_remove(&render_queue[owner_id], gfx_obj_id);
    if (remove_succes){
        id_free(gfx_obj_id);
    }

    // LOG_DEBUG("count after: %d", render_queue[owner_id].len);

    // if (render_queue[owner_id].len == 0){
    //     display_clear();
    //     display_update();
    // }
    //
    return remove_succes;
}

bool gfx_move_obj_coord_on_render_queue(QueueOwner owner_id, uint16_t gfx_obj_id, vector2_t vector_to_move){
    gfx_obj_t *gfx_obj_to_move = render_single_queue_map_get(&render_queue[owner_id], gfx_obj_id);
    if (gfx_obj_to_move != NULL){
        gfx_obj_to_move->coords.x += vector_to_move.x;
        gfx_obj_to_move->coords.y += vector_to_move.y;
        return true;
    } else {
        return false;
    }
}

vector2_t gfx_get_obj_coords(QueueOwner owner_id, uint16_t gfx_obj_id){
    return render_single_queue_map_get(&render_queue[owner_id], gfx_obj_id)->coords;
}

static const gfx_cmd_t knight_cmds[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};
static const metasprite_t knight = {2, knight_cmds};
gfx_obj_t knight_gfx_obj = {0, &knight, (vector2_t){110, 35}};

void gfx_system_init(){
    render_queue_mutex = xSemaphoreCreateRecursiveMutex();
    // gfx_add_to_render_queue(0, knight_gfx_obj);
    // add_to_render_queue(0, text_gfx_obj);
    xTaskCreate(gfx_task, "gfx", 256, NULL, 1, NULL);
}


