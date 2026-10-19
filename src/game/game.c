#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/utils/log.h"

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


static void game_task(void *p){
    LOG_DEBUG("start game task");
    for (;;){
        vTaskDelay(pdMS_TO_TICKS(2000));
        LOG_DEBUG("game task: try add to render queue");
        gfx_add_to_render_queue(0, test_knight_gfx_obj_1);
        vTaskDelay(pdMS_TO_TICKS(2000));
        gfx_add_to_render_queue(0, test_knight_gfx_obj_2);
        vTaskDelay(pdMS_TO_TICKS(2000));
        gfx_add_to_render_queue(0, test_knight_gfx_obj_3);

    }
}

void game_init(void){
    LOG_DEBUG("game init");
    gfx_add_to_render_queue(0, test_knight_gfx_obj);
    LOG_DEBUG("game init - add test to render queue");
    BaseType_t res = xTaskCreate(game_task, "game", 256, NULL, 1, NULL);
    LOG_DEBUG("game_task create result = %d", (int)res);
}
