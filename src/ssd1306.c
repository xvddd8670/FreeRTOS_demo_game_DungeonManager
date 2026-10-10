#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "dungeon_manager/ssd1306.h"
#include "dungeon_manager/i2c.h"

#define OLED_ADDR 0x78

extern I2C_HandleTypeDef hi2c1;   // определён в main.c

static uint8_t fb[8][129];        // [страница][0x40 + 128 байт данных]

static void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x00, cmd};
    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 100);
    if (st != HAL_OK)
        printf("cmd %02X st=%d err=0x%lX\n", cmd, st, hi2c1.ErrorCode);
}

void OLED_Clear(void)
{
    for (uint8_t p = 0; p < 8; p++) {
        fb[p][0] = 0x40;                  // control byte: данные
        memset(&fb[p][1], 0, 128);
    }
}

void OLED_Init(void)
{
    HAL_Delay(100);
    static const uint8_t seq[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x02, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
    };
    for (uint8_t i = 0; i < sizeof(seq); i++) OLED_WriteCmd(seq[i]);
    OLED_Clear();
}

void OLED_Pixel(uint8_t x, uint8_t y, uint8_t on)
{
    if (x >= 128 || y >= 64) return;
    if (on) fb[y / 8][1 + x] |=  (1 << (y % 8));
    else    fb[y / 8][1 + x] &= ~(1 << (y % 8));
}

void OLED_Update(void)
{
    for (uint8_t p = 0; p < 8; p++) {
        OLED_WriteCmd(0xB0 + p);
        OLED_WriteCmd(0x00);
        OLED_WriteCmd(0x10);
        I2C1_WriteDMA(OLED_ADDR, fb[p], 129);
    }
}
