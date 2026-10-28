#include "dungeon_manager/utils/log.h"
#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"
#include "dungeon_manager/game/game.h"
#include "dungeon_manager/game/camera.h"

vector2_t camera_coords = (vector2_t) {CAM_HALF_W, CAM_HALF_H};
uint8_t camera_move_px = 8;

void camera_init(void){
}

void camera_update(void){

}

static uint8_t comp_on_screen(uint8_t entity){
    uint8_t on_screen =
        tile_components[entity].coord.x <= camera_coords.x+CAM_HALF_W
        && tile_components[entity].coord.x >= camera_coords.x-CAM_HALF_W
        && tile_components[entity].coord.y <= camera_coords.y+CAM_HALF_H
        && tile_components[entity].coord.y >= camera_coords.y-CAM_HALF_H;
    return on_screen;
}

vector2_t check_direction(cam_direction_t direction){
    vector2_t direction_vec = {0, 0};
    switch (direction){
        case CAM_L:
            direction_vec.x -= camera_move_px;
            break;
        case CAM_R:
            direction_vec.x += camera_move_px;
            break;
        case CAM_U:
            direction_vec.y -= camera_move_px;
            break;
        case CAM_D:
            direction_vec.y += camera_move_px;
            break;
        default:
            break;
    }
    return direction_vec;
}

static void spawn_object(uint8_t entity_id, const metasprite_t *metasprite){

    gfx_obj_t buff_gfx_obj = {0, metasprite, (vector2_t){0, 0}};

    vector2_t diff = (vector2_t){
        tile_components[entity_id].coord.x - camera_coords.x+CAM_HALF_W,
        tile_components[entity_id].coord.y - camera_coords.y+CAM_HALF_H
    };
    buff_gfx_obj.coords = diff;
    tile_components[entity_id].gfx_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, buff_gfx_obj);
}

void camera_move(cam_direction_t direction){
    LOG_DEBUG("camera move");
    vector2_t direction_vec = check_direction(direction);

    camera_coords.x += direction_vec.x;
    camera_coords.y += direction_vec.y;

    for (uint8_t entity = 0; entity < MAX_ENTITIES; entity++){
        if (entities_masks[entity] != MASK_EMPTY){

            if (comp_on_screen(entity)){
                if (tile_components[entity].gfx_obj_id == GFX_OBJ_ID_RESET){
                    spawn_object(entity, &gfx_metasprites[GROUND_MS]);
                } else {
                    gfx_move_obj_coord_on_render_queue(
                        GAME_GFX_ID,
                        tile_components[entity].gfx_obj_id,
                        (vector2_t){direction_vec.x * -1, direction_vec.y * -1}
                    );
                }
            } else {
                if (tile_components[entity].gfx_obj_id != GFX_OBJ_ID_RESET){
                    gfx_remove_from_render_queue(GAME_GFX_ID, tile_components[entity].gfx_obj_id);
                    tile_components[entity].gfx_obj_id = GFX_OBJ_ID_RESET;
                }
            }
        }
    }
}
