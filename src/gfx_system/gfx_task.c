#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/gfx_system/gfx.h"
void gfx_task(void *arg){
    for (;;) {

        display_lock();
        display_clear();
        for (uint8_t i = 0; i < QUEUES_OWNER_COUNT; i++){
            render_single_queue_map *single_render_queue = &render_queue[i];
            for (uint8_t o = 0; o < single_render_queue->len; o++){
                gfx_draw_metasprite(&single_render_queue->items[o].value);
            }
            
        }

        display_unlock();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
