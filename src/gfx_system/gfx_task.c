#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/utils/log.h"

#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/display_system/display_manager.h"


void gfx_task(void *arg){
    display_lock();
    display_clear();
    display_unlock();
    for (;;) {

        gfx_render_queue_lock();
        display_lock();
        display_clear();
        for (uint8_t i = 0; i < QUEUES_OWNER_COUNT; i++){
            render_single_queue_map *single_render_queue = &render_queue[i];
            for (uint8_t o = 0; o < single_render_queue->len; o++){
                gfx_draw_metasprite(
                    single_render_queue->items[o].value.metasprite,
                    single_render_queue->items[o].value.coords);
            }
            
        }

        display_unlock();
        gfx_render_queue_unlock();

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
