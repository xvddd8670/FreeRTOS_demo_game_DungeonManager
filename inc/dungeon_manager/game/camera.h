#pragma once

#include "dungeon_manager/globals/common_types.h"

#define CAM_W 128
#define CAM_H 64
#define CAM_HALF_W (64)
#define CAM_HALF_H (32)

typedef enum { CAM_L, CAM_R, CAM_U, CAM_D } cam_direction_t;

extern vector2_t camera_coords;
extern uint8_t camera_move_px;

//static uint8_t comp_on_screen(uint8_t entity);
void camera_move(cam_direction_t direction);
void camera_init(void);
void camera_update(void);
