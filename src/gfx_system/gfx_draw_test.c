#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/gfx_system/gfx_draw.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"


static const gfx_cmd_t scene[] = {
    { .kind = CMD_HLINE,   .on = 1, .p.rect    = {0, 0, 128, 0} },
    { .kind = CMD_HLINE,   .on = 1, .p.rect    = {0, 63, 128, 0} },
    { .kind = CMD_VLINE,   .on = 1, .p.rect    = {0, 0, 0, 64} },
    { .kind = CMD_VLINE,   .on = 1, .p.rect    = {127, 0, 0, 64} },
    { .kind = CMD_PATTERN, .on = 1, .p.pattern = {10, 10, 40, 20, DISPLAY_PAT_50} },
    { .kind = CMD_LINE,    .on = 1, .p.line    = {60, 10, 120, 55} },
    { .kind = CMD_SPRITE,  .on = 1, .p.sprite  = {100, 5, SPR_HEART, 0} },
    { .kind = CMD_TEXT,    .on = 1, .p.text    = {4, 40, FONT_SMALL, "HI"} },
    { .kind = CMD_SHAPE,   .on = 1, .p.shape   = {70, 20, SHAPE_ARROW} },
};

static gfx_cmd_t box = { .kind = CMD_RECT, .on = 1, .p.rect = {0, 50, 8, 8} };


static void gfx_draw_test_task(void *arg) {
    int16_t x = 0, dx = 2;
    for (;;) {
        x += dx;
        if (x < 1 || x > 118) dx = -dx;
        box.p.rect.x = x;

        display_lock();                              // кадр целиком, чтобы display_task не отправил половину
        display_clear();
        for (uint8_t i = 0; i < sizeof(scene) / sizeof(scene[0]); i++) gfx_draw_cmd(&scene[i]);
        gfx_draw_cmd(&box);
        display_unlock();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void gfx_draw_test_start(void) {
    xTaskCreate(gfx_draw_test_task, "gfx_draw_test", 256, NULL, 1, NULL);
}
