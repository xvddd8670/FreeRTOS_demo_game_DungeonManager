#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/utils/log.h"
#include "dungeon_manager/periph/keypad.h"

#include "dungeon_manager/game/game.h"
#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"

static const gfx_cmd_t test_knight_cmds[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};
// static const metasprite_t test_knight = {2, test_knight_cmds};
static const metasprite_t test_knight = {2, test_knight_cmds};

gfx_obj_t test_knight_gfx_obj = {0, &test_knight, (vector2_t){30, 35}};
gfx_obj_t test_knight_gfx_obj_1 = {0, &test_knight, (vector2_t){60, 35}};
gfx_obj_t test_knight_gfx_obj_2 = {0, &test_knight, (vector2_t){90, 35}};
gfx_obj_t test_knight_gfx_obj_3 = {0, &test_knight, (vector2_t){110, 35}};


typedef struct {

} game_entity_t;


static void game_task(void *p){
    LOG_DEBUG("start game task");
    char key_code = 0;
    
    for (;;){
        vTaskDelay(pdMS_TO_TICKS(50));
        key_code = keypad_poll();
        if (key_code > 0){
            if (key_code == '*'){
                gfx_move_obj_coord_on_render_queue(0, 0, (vector2_t){5, 3});
            } else if (key_code == '0') {
                gfx_move_obj_coord_on_render_queue(0, 0, (vector2_t){-5, -3});
            }

        }
        // LOG_DEBUG("game task: try add to render queue");
        // gfx_add_to_render_queue(0, test_knight_gfx_obj_1);
        // vTaskDelay(pdMS_TO_TICKS(2000));
        // gfx_add_to_render_queue(0, test_knight_gfx_obj_2);
        // vTaskDelay(pdMS_TO_TICKS(2000));
        // gfx_add_to_render_queue(0, test_knight_gfx_obj_3);

    }
}

#define grid_lines_num 10
#define grid_size 16
void game_init(void){
    static gfx_cmd_t test_grid[grid_lines_num*2];
    for (int i = 0; i < grid_lines_num; i++){
        test_grid[i] = (gfx_cmd_t) { .type = CMD_LINE, .color = 1, .payload.line = { i*grid_size, 0, i*grid_size, 64}};
        test_grid[i+10] = (gfx_cmd_t) { .type = CMD_LINE, .color = 1, .payload.line = { 0, i*grid_size, 128, i*grid_size}};
    }


    // static const gfx_cmd_t test_grid[] = {
    //     { .type = CMD_LINE, .color = 1, .payload.line = { 10, 10, 200, 200} },
    //     { .type = CMD_LINE, .color = 1, .payload.line = {60, 10, 120, 55} },
    // };
    static const metasprite_t test_grid_ms = {grid_lines_num*2, test_grid};
    gfx_obj_t test_grid_obj = {7, &test_grid_ms, (vector2_t){0, 0}};

    // gfx_add_to_render_queue(0, test_knight_gfx_obj);
    uint8_t test_obj_id = 0;
    test_obj_id = gfx_add_to_render_queue(0, test_grid_obj);
    LOG_DEBUG("%d", test_obj_id);

    BaseType_t res = xTaskCreate(game_task, "game", 256, NULL, 1, NULL);
    LOG_DEBUG("game_task create result = %d", (int)res);
    gfx_move_obj_coord_on_render_queue(0, test_obj_id, (vector2_t){13, 3});
}
