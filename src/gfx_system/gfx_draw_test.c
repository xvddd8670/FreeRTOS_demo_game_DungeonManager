#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"
#include "dungeon_manager/globals/common_types.h"


static const gfx_cmd_t scene[] = {
    { .type = CMD_HLINE,   .color = 1, .payload.rect    = {0, 0, 128, 0} },
    { .type = CMD_HLINE,   .color = 1, .payload.rect    = {0, 63, 128, 0} },
    { .type = CMD_VLINE,   .color = 1, .payload.rect    = {0, 0, 0, 64} },
    { .type = CMD_VLINE,   .color = 1, .payload.rect    = {127, 0, 0, 64} },
    { .type = CMD_PATTERN, .color = 1, .payload.pattern = {10, 10, 40, 20, DISPLAY_PAT_50} },
    { .type = CMD_LINE,    .color = 1, .payload.line    = {60, 10, 120, 55} },
    { .type = CMD_SPRITE,  .color = 1, .payload.sprite  = {100, 5, SPR_HEART, 0} },
    { .type = CMD_TEXT,    .color = 1, .payload.text    = {4, 40, FONT_SMALL, "HI"} },
    { .type = CMD_SHAPE,   .color = 1, .payload.shape   = {70, 30, SHAPE_ARROW} },
};

static const metasprite_t scene_ms = {sizeof(scene) / sizeof(scene[0]), scene, (vector2_t){.x=0, .y=0}};

static const gfx_cmd_t knight_cmds[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};
static const metasprite_t knight = {2, knight_cmds, (vector2_t){0, 0}};

static const gfx_cmd_t box = { .type = CMD_RECT, .color = 1, .payload.rect = {0, 50, 8, 8} };

static void gfx_draw_test_task(void *arg) {
    int16_t x = 0, dx = 2;
    for (;;) {
        x += dx;
        if (x < 1 || x > 118) dx = -dx;
        // box.payload.rect.x = x;

        display_lock();                              // кадр целиком, чтобы display_task не отправил половину
        display_clear();
        for (uint8_t i = 0; i < sizeof(scene) / sizeof(scene[0]); i++) gfx_draw_one_cmd(&scene[i], 0, 0);
        // gfx_draw_cmd(&box);
        // gfx_draw_metasprite(&scene_ms, 3, 3);
        gfx_draw_one_cmd(&box, x, 0);
        gfx_draw_metasprite(&knight);
        display_unlock();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void gfx_draw_test_start(void) {
    xTaskCreate(gfx_draw_test_task, "gfx_draw_test", 256, NULL, 1, NULL);
}
