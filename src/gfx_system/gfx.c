#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_task.h"

render_single_queue_map render_queue[QUEUES_OWNER_COUNT];

bool add_to_render_queue(QueueOwner owner_id, uint8_t obj_addr_as_id, Metasprite metasprite){
    return render_single_queue_map_put(&render_queue[owner_id], obj_addr_as_id, metasprite);
}

bool remove_from_render_queue(QueueOwner owner_id, uint8_t obj_addr_as_id){
    return render_single_queue_map_remove(&render_queue[owner_id], obj_addr_as_id);
}

bool move_obj_coord_on_render_queue(QueueOwner owner_id, uint8_t obj_addr_as_id, Vector2 vector_to_move){
    Metasprite *ms_to_move = render_single_queue_map_get(&render_queue[owner_id], obj_addr_as_id);
    if (ms_to_move != NULL){
        ms_to_move->ms_coords.x += vector_to_move.x;
        ms_to_move->ms_coords.y += vector_to_move.y;
        return true;
    } else {
        return false;
    }
}

void gfx_system_init(){
    xTaskCreate(gfx_task, "gfx", 512, NULL, 1, NULL);
}

// static void gfx_draw_test_task(void *arg) {
//     for (;;) {
//
//         display_lock();
//         display_clear();
//         for (uint8_t i = 0; i < QUEUES_OWNERS_COUNT; i++){
//             ;
//             for (uint8_t y = 0; y < 10; i++) {
//                 ;
//
//             }
//
//
//         }
//
//         display_unlock();
//
//         vTaskDelay(pdMS_TO_TICKS(50));
//     }
// }
