#include "FreeRTOS.h"
#include "task.h"
#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/display_system/ssd1306.h"

void display_task(void *p) {
    display_init();
    vTaskDelay(pdMS_TO_TICKS(100));

    for (;;) {
        need_update_take(portMAX_DELAY);
        display_update();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
