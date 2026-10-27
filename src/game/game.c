#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/utils/log.h"
#include "dungeon_manager/periph/keypad.h"

#include "dungeon_manager/game/game.h"
#include "dungeon_manager/game/camera.h"
#include "dungeon_manager/gfx_system/gfx.h"
#include "dungeon_manager/gfx_system/gfx_resources.h"



#define grid_lines_num 10
#define grid_size 16

uint8_t current_floor[FLOOR_W][FLOOR_H];
uint16_t entities_masks[MAX_ENTITIES];
ground_component ground_components[MAX_ENTITIES];
corridor_component corridor_components[MAX_ENTITIES];
generator_component generator_components[MAX_ENTITIES];
processor_component processor_components[MAX_ENTITIES];
elevator_component elevator_components[MAX_ENTITIES];
tile_component tile_components[MAX_ENTITIES];


static gfx_cmd_t test_grid[grid_lines_num*2];
static const metasprite_t test_grid_ms = {grid_lines_num*2, test_grid};
//******************************************************
// test graphics
//******************************************************
static const gfx_cmd_t test_knight_cmds[] = {
    { .type = CMD_SPRITE, .color = 1, .payload.sprite = {-4, -4, SPR_HEART, 0} },   // спрайт 8x8, центр в pivot
    { .type = CMD_LINE,   .color = 1, .payload.line   = { 4,  0, 12, 0} },          // линия вправо от pivot
};
// static const metasprite_t test_knight = {2, test_knight_cmds};
static const metasprite_t test_knight = {2, test_knight_cmds};

gfx_obj_t test_knight_gfx_obj = {0, &gfx_metasprites[KNIGHT_MS], (vector2_t){0, 0}};
gfx_obj_t test_knight_gfx_obj_1 = {0, &test_knight, (vector2_t){60, 35}};
gfx_obj_t test_knight_gfx_obj_2 = {0, &test_knight, (vector2_t){90, 35}};
gfx_obj_t test_knight_gfx_obj_3 = {0, &test_knight, (vector2_t){110, 35}};


static char test_text[12] = "HELLO";
static const gfx_cmd_t test_text_cmds[] = {
    { .type = CMD_TEXT,    .color = 1, .payload.text    = {0, 0, FONT_SMALL, test_text} },
};

static const metasprite_t test_text_ms = {1, test_text_cmds};

gfx_obj_t test_text_obj = {5, &test_text_ms, (vector2_t){30, 35}};
/*********************************************************/

uint8_t comp_on_screen(uint8_t entity){
    uint8_t on_screen =
        tile_components[entity].coord.x <= camera_coords.x+64
        && tile_components[entity].coord.x >= camera_coords.x-64
        && tile_components[entity].coord.y <= camera_coords.y+32
        && tile_components[entity].coord.y >= camera_coords.y-32;
    return on_screen;
}

void camera_move(cam_direction_t direction){
    LOG_DEBUG("CAMERA MOVE:");
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
    camera_coords.x += direction_vec.x;
    camera_coords.y += direction_vec.y;

    for (uint8_t entity = 0; entity < MAX_ENTITIES; entity++){
        if (entities_masks[entity] != 0){
            // LOG_DEBUG("TC coords - x: %d, y: %d", tile_components[entity].coord.x, tile_components[entity].coord.y);
            LOG_DEBUG("CAM coords - x: %d, y: %d", camera_coords.x, camera_coords.y);
            LOG_DEBUG("comp on screen bool: %d", comp_on_screen(entity));
            LOG_DEBUG("gfx obj id: %d", tile_components[entity].gfx_obj_id);
            if (tile_components[entity].gfx_obj_id != 0){
                vector2_t gfx_coords = gfx_get_obj_coords(GAME_GFX_ID, tile_components[entity].gfx_obj_id);
                LOG_DEBUG("GFX coords - x: %d, y: %d", gfx_coords.x, gfx_coords.y);
            }

            if (comp_on_screen(entity)){

                if (tile_components[entity].gfx_obj_id == 0){
                    LOG_DEBUG("spawn gfx obj");
                    gfx_obj_t buff_gfx_obj = test_knight_gfx_obj;

                    vector2_t diff = (vector2_t){
                        tile_components[entity].coord.x - camera_coords.x+64,
                        tile_components[entity].coord.y - camera_coords.y+32
                    };
                    buff_gfx_obj.coords = diff;
                    tile_components[entity].gfx_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, buff_gfx_obj);
                } else {
                    LOG_DEBUG("move gfx obj");
                    gfx_move_obj_coord_on_render_queue(
                        GAME_GFX_ID,
                        tile_components[entity].gfx_obj_id,
                        (vector2_t){direction_vec.x * -1, direction_vec.y * -1}
                    );

                }
            } else {
                LOG_DEBUG("despawn gfx obj");
                gfx_remove_from_render_queue(GAME_GFX_ID, tile_components[entity].gfx_obj_id);
                tile_components[entity].gfx_obj_id = 0;

            }
        }
    }

}

static void game_task(void *p){
    LOG_DEBUG("start game task");
    char key_code = 0;
    
    for (;;){
        vTaskDelay(pdMS_TO_TICKS(50));
        key_code = keypad_poll();
        if (key_code > 0){
            if (key_code == '*'){
                strcpy(test_text, "HELL");
                // gfx_move_obj_coord_on_render_queue(0, 0, (vector2_t){5, 3});
            } else if (key_code == '0') {
                strcpy(test_text, "HELLO WORLD");
                // gfx_move_obj_coord_on_render_queue(0, 0, (vector2_t){-5, -3});
            } else if (key_code == '4') {
                camera_move(CAM_L);
            } else if (key_code == '6') {
                camera_move(CAM_R);
            } else if (key_code == '2') {
                camera_move(CAM_U);
            } else if (key_code == '8') {
                camera_move(CAM_D);
            }

        }
        // LOG_DEBUG("game task: try add to render queue");
        // gfx_add_to_render_queue(0, test_knight_gfx_obj_1);
        // vTaskDelay(pdMS_TO_TICKS(2000));
        // gfx_add_to_render_queue(0, test_knight_gfx_obj_2);
        // vTaskDelay(pdMS_TO_TICKS(2000));
        // gfx_add_to_render_queue(0, test_knight_gfx_obj_3);

    }
}

gfx_obj_t test_grid_obj = {0, &test_grid_ms, (vector2_t){0, 0}};
uint16_t create_grid(void){
    for (int i = 0; i < grid_lines_num; i++){
        test_grid[i] = (gfx_cmd_t) { .type = CMD_LINE, .color = 1, .payload.line = { i*grid_size, 0, i*grid_size, 64}};
        test_grid[i+10] = (gfx_cmd_t) { .type = CMD_LINE, .color = 1, .payload.line = { 0, i*grid_size, 128, i*grid_size}};
    }
    return gfx_add_to_render_queue(GAME_GFX_ID, test_grid_obj);
}

void create_entity(void){
    uint16_t comp_need = 0;
    for (uint8_t entity = 0; entity < MAX_ENTITIES; entity++){
        if (entities_masks[entity] == 0){

        }
    }
}

void add_entity_to_gfx_system(uint8_t entity_id){

}

void game_init(void){
    uint8_t test_obj_id = 0;
    gfx_obj_t buff_gfx_obj = test_knight_gfx_obj;
    // ---------------------------


    uint8_t entity_id = 0;
    uint8_t floor_counter = 0;
    vector2_t entity_coord = (vector2_t) {0, 0};

    entities_masks[entity_id] |= (cb_alive | cb_ground);

    ground_components[entity_id].ground_type = 0;
    tile_components[entity_id].coord = entity_coord;
    tile_components[entity_id].floor = floor_counter;

    current_floor[entity_coord.x][entity_coord.y] = entity_id;

    buff_gfx_obj.coords = tile_components[entity_id].coord;
    tile_components[entity_id].gfx_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, buff_gfx_obj);
    
    // ---------------------------
    // test_obj_id = create_grid();
    // test_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, test_grid_obj);

    buff_gfx_obj.coords = (vector2_t) {100, 50};
    gfx_add_to_render_queue(GAME_GFX_ID, buff_gfx_obj);
    LOG_DEBUG("%d", test_obj_id);

    // test_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, test_text_obj);

    BaseType_t res = xTaskCreate(game_task, "game", 256, NULL, 1, NULL);
    LOG_DEBUG("game_task create result = %d", (int)res);
    // gfx_move_obj_coord_on_render_queue(GAME_GFX_ID, test_obj_id, (vector2_t){13, 3});
}

void calculate_base_production(void){
    uint16_t comp_need = cb_alive;
    for (uint8_t entity = 0; entity < MAX_ENTITIES; entity++){
        if ((entities_masks[entity] & comp_need) == comp_need){

        }
    }
}
