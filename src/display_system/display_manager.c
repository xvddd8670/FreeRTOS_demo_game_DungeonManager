#include <stdint.h>
#include "dungeon_manager/ssd1306.h"


void display_init(void)
{
    OLED_Init();
}


void display_clear(void)
{
    OLED_Clear();
}


void display_pixel(uint8_t x, uint8_t y, uint8_t pixel_on_off)
{
    OLED_Pixel(x, y, pixel_on_off);
}


void display_update(void)
{
    OLED_Update();

}


