#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "dungeon_manager/utils/static_containers.h"
#include "dungeon_manager/gfx_system/gfx_types.h"


#ifndef GFX_FRAME_MS
#define GFX_FRAME_MS 33     // период кадра (~30 fps)
#endif

#define RENDER_QUEUE_COUNT 64
#define QUEUES_OWNER_COUNT 2

typedef enum {
    GAME,
    MAIN_MENU
}QueueOwner;

extern xSemaphoreHandle render_queue_mutex;
void gfx_render_queue_lock(void);
void gfx_render_queue_unlock(void);

define_static_map(render_single_queue_map, uint16_t, gfx_obj_t, RENDER_QUEUE_COUNT, uint16_t, cmp_scalar)
extern render_single_queue_map render_queue[QUEUES_OWNER_COUNT];

int16_t gfx_add_to_render_queue(QueueOwner owner_id, gfx_obj_t gfx_obj);
void gfx_system_init();
