#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/i2c.h"
#include "dungeon_manager/display_system/display_manager.h"
#include "dungeon_manager/display_system/display_task.h"
#include "dungeon_manager/log.h"
#include "dungeon_manager/cli.h"
#include "dungeon_manager/display_system/ssd1306.h"

//void SysTick_Handler(void) { HAL_IncTick(); }
extern void xPortSysTickHandler(void);
void SysTick_Handler(void) {
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        xPortSysTickHandler();
}

static void LedTask(void *p) {
    for (;;) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void test_display_task(void *p) {

    for (uint8_t x = 0; x < 128; x++) display_pixel(x, 32, 1);

    int test_anim_x = 10;
    int test_anim_y = 10;
    for (;;) {
        display_clear();
        test_anim_x += 1;
        if (test_anim_x >= 128) {
            test_anim_x = 0;
            test_anim_y += 10;
        }
        if (test_anim_y >= 64){test_anim_y = 0;}
        for (int i=0; i<5; i++) {display_pixel(test_anim_x+i, test_anim_y+i, 1);}

        display_fill_rect(10, 10, 20, 20, 1);
        display_line(30, 30, 110, 85, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// --------------------------------
// main
// --------------------------------
int main(void) {

    Log_Init();
    LOG_INFO("**********************");
    LOG_INFO("******start main******");

    LOG_INFO("HAL init");
    HAL_Init();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef g = {.Pin = GPIO_PIN_8, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_LOW};

    HAL_GPIO_Init(GPIOA, &g);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

    LOG_INFO("I2C init");
    I2C1_Init();

    LOG_INFO("display init");
    display_manager_init();

    LOG_INFO("create tasks");
    xTaskCreate(LedTask, "led", 128, NULL, 1, NULL);
    xTaskCreate(CLI_Task, "cli", 512, NULL, 1, NULL);
    xTaskCreate(test_display_task, "test_d_task", 256, NULL, 1, NULL);
    LOG_INFO("start task scheduler");
    vTaskStartScheduler();


    while(true) { }
}
