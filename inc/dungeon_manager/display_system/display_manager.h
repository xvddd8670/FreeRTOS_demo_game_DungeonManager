/*
 * display_manager.h
 *
 * implements the use of the display driver by abstracting work with it
 * allowing you to connect different displays
 *
 * you need to call the display_manager_init() in main
 */

#pragma once

#include <stdint.h>

#include "FreeRTOS.h"
#include "semphr.h"


#define DISPLAY_W 128
#define DISPLAY_H 64


void need_update_set(void);
bool need_update_take(TickType_t timeout);

void display_lock(void);
void display_unlock(void);

void display_manager_init(void);
void display_init(void);
void display_clear(void);
void display_update(void);

void display_pixel(uint8_t x, uint8_t y, uint8_t pixel_on_off);
