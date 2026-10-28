#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/display_system/ssd1306.h"
#include "dungeon_manager/display_system/display_task.h"

#include "dungeon_manager/utils/log.h"


static xSemaphoreHandle display_mutex;
static xSemaphoreHandle need_update_sem;

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

uint16_t display_width(void)  { return OLED_W; }
uint16_t display_height(void) { return OLED_H; }

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

void display_pixel(int16_t x, int16_t y, color_t c) {
    if (x < 0 || y < 0 || x >= display_width() || y >= display_height()) return;
    display_lock();
    oled_pixel(x, y, c);
    display_unlock();
    need_update_set();
}

static bool clip_rect(int16_t *x, int16_t *y, int16_t *w, int16_t *h) {
    if (*w <= 0 || *h <= 0) return false;
    int x1 = *x + *w, y1 = *y + *h;
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (x1 > display_width())  x1 = display_width();
    if (y1 > display_height()) y1 = display_height();
    if (*x >= x1 || *y >= y1) return false;
    *w = x1 - *x;
    *h = y1 - *y;
    return true;
}

void display_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, color_t c) {
    if (!clip_rect(&x, &y, &w, &h)) return;
    display_lock();
    oled_fill_rect(x, y, w, h, c);
    display_unlock();
    need_update_set();
}


void display_fill_rect_pat(int16_t x, int16_t y, int16_t w, int16_t h, display_pattern_t pat, color_t c) {
    if (pat >= DISPLAY_PAT_COUNT) pat = DISPLAY_PAT_SOLID;
    display_lock();
    oled_fill_rect_pat(x, y, w, h, pat_table[pat], c);
    display_unlock();
    need_update_set();
}

void display_hline(int16_t x, int16_t y, int16_t w, color_t c) {
    int16_t h = 1;
    if (!clip_rect(&x, &y, &w, &h)) return;
    display_lock();
    oled_hline(x, y, w, c);
    display_unlock();
    need_update_set();
}

void display_vline(int16_t x, int16_t y, int16_t h, color_t c){
    display_lock();
    oled_vline(x, y, h, c);
    display_unlock();
    need_update_set();
}

void display_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t on) {
    display_lock();
    oled_line(x0, y0, x1, y1, on);
    display_unlock();
    need_update_set();
}

void display_blit_mono(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, color_t c) {
    display_lock();
    oled_blit_mono(x, y, w, h, bmp, c);
    display_unlock();
    need_update_set();
}

void display_update(void) {
    display_lock();
    oled_update();
    display_unlock();
}
