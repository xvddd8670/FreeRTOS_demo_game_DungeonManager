#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/display_system/ssd1306.h"
#include "dungeon_manager/display_system/display_task.h"

#include "dungeon_manager/log.h"


static xSemaphoreHandle display_mutex;
static xSemaphoreHandle need_update_sem;


void display_lock(void) { xSemaphoreTakeRecursive(display_mutex, portMAX_DELAY); }
void display_unlock(void){ xSemaphoreGiveRecursive(display_mutex); }

void need_update_set(void) { xSemaphoreGive(need_update_sem); }
bool need_update_take(TickType_t timeout) { return xSemaphoreTake(need_update_sem, timeout) == pdTRUE; }

void display_manager_init(void) {
    need_update_sem = xSemaphoreCreateBinary();
    display_mutex = xSemaphoreCreateRecursiveMutex();
    xTaskCreate(display_task, "display", 256, NULL, 1, NULL);
}

void display_init(){
    display_lock();
    OLED_Init();
    display_unlock();
}

void display_clear(void) {
    display_lock();
    OLED_Clear();
    display_unlock();
}

void display_pixel(uint8_t x, uint8_t y, uint8_t pixel_on_off) {
    need_update_set();
    display_lock();
    OLED_Pixel(x, y, pixel_on_off);
    display_unlock();
}

void display_update(void) {
    display_lock();
    OLED_Update();
    display_unlock();
}
