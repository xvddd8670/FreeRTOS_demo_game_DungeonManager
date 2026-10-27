#pragma once

#include "dungeon_manager/globals/common_types.h"

typedef enum { CAM_L, CAM_R, CAM_U, CAM_D } cam_direction_t;

extern vector2_t camera_coords;
extern uint8_t camera_move_px;

void camera_init(void);
void camera_update(void);
