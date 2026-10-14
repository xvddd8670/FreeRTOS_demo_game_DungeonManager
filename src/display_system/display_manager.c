#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/display_system/ssd1306.h"
#include "dungeon_manager/display_system/display_task.h"

#include "dungeon_manager/utils/log.h"


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
    oled_init();
    display_unlock();
}

void display_clear(void) {
    display_lock();
    oled_clear();
    display_unlock();
    // need_update_set();
}

void display_pixel(uint8_t x, uint8_t y, uint8_t pixel_on_off) {
    need_update_set();
    display_lock();
    oled_pixel(x, y, pixel_on_off);
    display_unlock();
}

void display_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on) {
    display_lock();
    oled_fill_rect(x, y, w, h, on);
    display_unlock();
    need_update_set();
}

static const uint8_t *const pat_table[DISPLAY_PAT_COUNT] = {
    [DISPLAY_PAT_SOLID]       = oled_pat[OLED_PAT_SOLID],
    [DISPLAY_PAT_75]          = oled_pat[OLED_PAT_75],
    [DISPLAY_PAT_50]          = oled_pat[OLED_PAT_50],
    [DISPLAY_PAT_25]          = oled_pat[OLED_PAT_25],
    [DISPLAY_PAT_12]          = oled_pat[OLED_PAT_12],
    [DISPLAY_PAT_HLINES]      = oled_pat[OLED_PAT_HLINES],
    [DISPLAY_PAT_HLINES_WIDE] = oled_pat[OLED_PAT_HLINES_WIDE],
    [DISPLAY_PAT_VLINES]      = oled_pat[OLED_PAT_VLINES],
    [DISPLAY_PAT_VLINES_WIDE] = oled_pat[OLED_PAT_VLINES_WIDE],
    [DISPLAY_PAT_DIAG_L]      = oled_pat[OLED_PAT_DIAG_L],
    [DISPLAY_PAT_DIAG_R]      = oled_pat[OLED_PAT_DIAG_R],
    [DISPLAY_PAT_GRID]        = oled_pat[OLED_PAT_GRID],
};

void display_fill_rect_pat(uint8_t x, uint8_t y, uint8_t w, uint8_t h, display_pattern_t pat, uint8_t on) {
    if (pat >= DISPLAY_PAT_COUNT) pat = DISPLAY_PAT_SOLID;
    display_lock();
    oled_fill_rect_pat(x, y, w, h, pat_table[pat], on);
    display_unlock();
    need_update_set();
}

void display_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t on) {
    display_lock();
    oled_hline(x, y, w, on);
    display_unlock();
    need_update_set();
}

void display_vline(uint8_t x, uint8_t y, uint8_t h, uint8_t on) {
    display_lock();
    oled_vline(x, y, h, on);
    display_unlock();
    need_update_set();
}

void display_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t on) {
    display_lock();
    oled_line(x0, y0, x1, y1, on);
    display_unlock();
    need_update_set();
}

static oled_blit_mode_t to_oled_mode(display_blit_mode_t mode) {
    switch (mode) {
    case DISPLAY_BLIT_XOR:   return OLED_BLIT_XOR;
    case DISPLAY_BLIT_CLEAR: return OLED_BLIT_CLEAR;
    default:                 return OLED_BLIT_OR;
    }
}

void display_blit(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, display_blit_mode_t mode) {
    display_lock();
    oled_blit(x, y, w, h, bmp, to_oled_mode(mode));
    display_unlock();
    need_update_set();
}

void display_update(void) {
    display_lock();
    oled_update();
    display_unlock();
}

// void display_update(void) {
//     display_lock();
//     OLED_Snapshot();
//     display_unlock();
//     OLED_Send();
// }
