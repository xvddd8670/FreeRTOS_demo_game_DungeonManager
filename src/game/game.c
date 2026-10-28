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
static char test_text[12] = "HELLO";
static const gfx_cmd_t test_text_cmds[] = {
    { .type = CMD_TEXT,    .color = 1, .payload.text    = {0, 0, FONT_SMALL, test_text} },
};

static const metasprite_t test_text_ms = {1, test_text_cmds};

gfx_obj_t test_text_obj = {5, &test_text_ms, (vector2_t){30, 35}};
/*********************************************************/

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

static void fill_floor(void){
    uint8_t entity_id = 0;
    uint8_t floor_counter = 0;


    // for (int i = 0; i < FLOOR_W; i++){
    //     for (int j = 0; j < FLOOR_H; j++){
    //         vector2_t entity_coord = (vector2_t) {i * 8, j * 8};
    //
    //     }
    //
    // }
    
    vector2_t entity_coord = (vector2_t) {20, 20};

    entities_masks[entity_id] |= (cb_alive | cb_ground);

    ground_components[entity_id].ground_type = 0;
    tile_components[entity_id].coord = entity_coord;
    tile_components[entity_id].floor = floor_counter;
    tile_components[entity_id].gfx_obj_id = -1;

    current_floor[entity_coord.x][entity_coord.y] = entity_id;
   
}

void game_init(void){
    uint8_t test_obj_id = 0;
    gfx_obj_t buff_gfx_obj = {0, &gfx_metasprites[GROUND_MS], (vector2_t){0, 0}};
    
    // ---------------------------

    fill_floor();

    // tile_components[entity_id].gfx_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, buff_gfx_obj);
    
    // ---------------------------

    // test_obj_id = create_grid();
    // test_obj_id = gfx_add_to_render_queue(GAME_GFX_ID, test_grid_obj);

    buff_gfx_obj.coords = (vector2_t) {100, 50};
    // gfx_add_to_render_queue(GAME_GFX_ID, buff_gfx_obj);
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
