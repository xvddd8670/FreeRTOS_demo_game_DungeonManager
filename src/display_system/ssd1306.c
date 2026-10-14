#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f1xx_hal.h"
#include "dungeon_manager/display_system/ssd1306.h"
#include "dungeon_manager/periph/i2c.h"
#include "dungeon_manager/utils/log.h"


#define OLED_ADDR 0x78


extern I2C_HandleTypeDef hi2c1;

static uint8_t fb_draw[128 * 8];       // сюда рисуем, только пиксели
static uint8_t fb_dma[1 + 128 * 8];    // снимок для отправки, fb_dma[0] = control byte


// узоры: 8 колонок, бит 0 сверху, привязаны к экрану (pat[x & 7])
const uint8_t oled_pat[OLED_PAT_COUNT][8] = {
    [OLED_PAT_SOLID]       = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    [OLED_PAT_75]          = {0xAA, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF},
    [OLED_PAT_50]          = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55},
    [OLED_PAT_25]          = {0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00},
    [OLED_PAT_12]          = {0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
    [OLED_PAT_HLINES]      = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55},
    [OLED_PAT_HLINES_WIDE] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
    [OLED_PAT_VLINES]      = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00},
    [OLED_PAT_VLINES_WIDE] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00},
    [OLED_PAT_DIAG_L]      = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80},
    [OLED_PAT_DIAG_R]      = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01},
    [OLED_PAT_GRID]        = {0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
};


static inline void set_bits(uint8_t *p, uint8_t mask, uint8_t on) {
    if (on) *p |= mask;
    else    *p &= ~mask;
}

static inline void swap_i16(int16_t *a, int16_t *b) {
    int16_t t = *a; *a = *b; *b = t;
}


static void oled_write_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    HAL_StatusTypeDef st = I2C1_WriteDMA(OLED_ADDR, buf, 2);
    if (st != HAL_OK)
        LOG_ERROR("cmd %02X st=%d\n", cmd, st);
}

void oled_clear(void) {
    memset(fb_draw, 0, sizeof(fb_draw));
}

void oled_init(void) {
    static const uint8_t seq[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14,
        0x20, 0x00,                    // horizontal addressing
        0x21, 0x00, 0x7F,              // column 0..127
        0x22, 0x00, 0x07,              // page 0..7
        0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
    };
    for (uint8_t i = 0; i < sizeof(seq); i++) oled_write_cmd(seq[i]);
    fb_dma[0] = 0x40;                  // control byte, один раз
    oled_clear();
}

void oled_pixel(uint8_t x, uint8_t y, uint8_t on) {
    if (x >= 128 || y >= 64) return;
    uint16_t idx = x + (y / 8) * 128;
    if (on) fb_draw[idx] |=  (1 << (y % 8));
    else    fb_draw[idx] &= ~(1 << (y % 8));
}


// заливка с узором: бит узора 1 = рисовать цвет on, 0 = не трогать
void oled_fill_rect_pat(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *pat, uint8_t on) {
    if (x >= 128 || y >= 64 || !w || !h) return;
    if (x + w > 128) w = 128 - x;
    if (y + h > 64)  h = 64 - y;

    uint8_t end = y + h;
    while (y < end) {
        uint8_t top = y % 8;
        uint8_t rows = 8 - top;
        if (rows > end - y) rows = end - y;
        uint8_t mask = (uint8_t)(((1 << rows) - 1) << top);

        uint8_t *p = &fb_draw[(y / 8) * 128 + x];
        for (uint8_t i = 0; i < w; i++) {
            uint8_t bits = pat[(x + i) & 7] & mask;
            if (on) p[i] |= bits;
            else    p[i] &= ~bits;
        }
        y += rows;
    }
}

void oled_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on) {
    oled_fill_rect_pat(x, y, w, h, oled_pat[OLED_PAT_SOLID], on);
}


void oled_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t on) {
    if (x >= 128 || y >= 64 || !w) return;
    if (x + w > 128) w = 128 - x;
    uint8_t *p = &fb_draw[(y >> 3) * 128 + x];
    uint8_t m = 1 << (y & 7);
    if (on) {
        while (w--) *p++ |= m;
    } else {
        m = ~m;
        while (w--) *p++ &= m;
    }
}

void oled_vline(uint8_t x, uint8_t y, uint8_t h, uint8_t on) {
    if (x >= 128 || y >= 64 || !h) return;
    if (y + h > 64) h = 64 - y;
    uint8_t *p = &fb_draw[(y >> 3) * 128 + x];
    uint8_t top = y & 7;

    if (top) {                                  // неполная первая страница
        uint8_t rows = 8 - top;
        if (rows > h) rows = h;
        set_bits(p, ((1 << rows) - 1) << top, on);
        h -= rows;
        p += 128;
    }
    while (h >= 8) {                            // целые страницы без маски
        *p = on ? 0xFF : 0x00;
        p += 128;
        h -= 8;
    }
    if (h) set_bits(p, (1 << h) - 1, on);       // неполная последняя
}

void oled_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t on) {
    if (y0 == y1) {                             // hor
        if (y0 < 0 || y0 >= 64) return;
        if (x0 > x1) swap_i16(&x0, &x1);
        if (x1 < 0 || x0 >= 128) return;
        if (x0 < 0)   x0 = 0;
        if (x1 > 127) x1 = 127;
        oled_hline(x0, y0, x1 - x0 + 1, on);
        return;
    }
    if (x0 == x1) {                             // vert
        if (x0 < 0 || x0 >= 128) return;
        if (y0 > y1) swap_i16(&y0, &y1);
        if (y1 < 0 || y0 >= 64) return;
        if (y0 < 0)   y0 = 0;
        if (y1 > 63)  y1 = 63;
        oled_vline(x0, y0, y1 - y0 + 1, on);
        return;
    }

    int16_t dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;

    for (;;) {
        if ((uint16_t)x0 < 128 && (uint16_t)y0 < 64)
            oled_pixel(x0, y0, on);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


static void apply_byte(uint8_t *dst, uint8_t v, oled_blit_mode_t mode) {
    if (mode == OLED_BLIT_OR)       *dst |= v;
    else if (mode == OLED_BLIT_XOR) *dst ^= v;
    else                            *dst &= ~v;
}

void oled_blit(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmp, oled_blit_mode_t mode) {
    uint8_t pages = (h + 7) / 8;
    for (uint8_t sp = 0; sp < pages; sp++) {
        uint8_t src_mask = 0xFF;
        if (h - sp * 8 < 8) src_mask = (1 << (h - sp * 8)) - 1;   // лишние биты последней страницы

        int16_t dy = y + sp * 8;
        int16_t page = dy >> 3;
        uint8_t shift = dy & 7;
        if (page >= 8 || page < -1) continue;

        for (uint8_t i = 0; i < w; i++) {
            int16_t dx = x + i;
            if (dx < 0 || dx >= 128) continue;
            uint8_t b = bmp[sp * w + i] & src_mask;

            if (page >= 0)
                apply_byte(&fb_draw[page * 128 + dx], b << shift, mode);
            if (shift && page + 1 < 8)
                apply_byte(&fb_draw[(page + 1) * 128 + dx], b >> (8 - shift), mode);
        }
    }
}

void oled_update(void) {
    memcpy(&fb_dma[1], fb_draw, sizeof(fb_draw));
    I2C1_WriteDMA(OLED_ADDR, fb_dma, sizeof(fb_dma));
}
