#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/i2c.h"
#include "dungeon_manager/ssd1306.h"
#include "dungeon_manager/log.h"

//void SysTick_Handler(void) { HAL_IncTick(); }
extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        xPortSysTickHandler();
}


// -----------------------------
// rtos tasks
// -----------------------------
static void LedTask(void *p)
{
    for (;;) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


static void OledTask(void *p)
{
    OLED_Init();
    for (uint8_t x = 0; x < 128; x++) OLED_Pixel(x, 32, 1);

    int test_anim_x = 10;
    int test_anim_y = 10;

    for (;;) {
        OLED_Clear();
        test_anim_x += 1;
        if (test_anim_x >= 128){
            test_anim_x = 0;
            test_anim_y += 10;
        }
        if (test_anim_y >= 64){test_anim_y = 0;}
        for (int i=0; i<5; i++){OLED_Pixel(test_anim_x+i, test_anim_y+i, 1);}

        OLED_Update();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


// --------------------------------
// main
// --------------------------------
int main(void) {

    Log_Init();
    LOG_INFO("**********************");
    LOG_INFO("******start main******");

    LOG_INFO("start HAL init");
    HAL_Init();

    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef g = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_LOW};

    HAL_GPIO_Init(GPIOC, &g);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    I2C1_Init();

    LOG_INFO("create tasks");
    xTaskCreate(LedTask, "led", 512, NULL, 1, NULL);
    xTaskCreate(OledTask, "oled", 128, NULL, 1, NULL);
    LOG_INFO("start task scheduler");
    vTaskStartScheduler();


    while(true) { }
}
