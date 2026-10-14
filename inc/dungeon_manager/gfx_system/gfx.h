#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "dungeon_manager/gfx_system/gfx_draw.h"


// лимиты системы, можно переопределить до include
#ifndef GFX_ENTRIES
#define GFX_ENTRIES  6      // сколько записей (владельцев) одновременно
#endif
#ifndef GFX_CMDS
#define GFX_CMDS     8      // команд в одной записи
#endif
#ifndef GFX_LAYERS
#define GFX_LAYERS   4      // слои 0..GFX_LAYERS-1, больший рисуется поверх
#endif
#ifndef GFX_FRAME_MS
#define GFX_FRAME_MS 33     // период кадра (~30 fps)
#endif


// запуск: создаёт мьютекс и задачу рендера
void gfx_init(void);

// заменить набор команд владельца key целиком (key = адрес любой своей static переменной)
// cmds копируются, но строки и картинки внутри команд должны быть статическими
// false = нет места в таблице или n > GFX_CMDS
bool gfx_set(const void *key, uint8_t layer, const gfx_cmd_t *cmds, uint8_t n);

// заменить одну команду по номеру в наборе
void gfx_patch(const void *key, uint8_t idx, const gfx_cmd_t *c);

// скрыть или показать одну команду, номера остальных не меняются
void gfx_hide(const void *key, uint8_t idx, uint8_t hidden);

// убрать запись владельца целиком
void gfx_clear(const void *key);
