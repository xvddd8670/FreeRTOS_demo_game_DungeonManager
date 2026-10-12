#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "dungeon_manager/i2c.h"
#include "dungeon_manager/ssd1306.h"
#include "dungeon_manager/log.h"
#include "dungeon_manager/cli.h"

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
    xTaskCreate(LedTask, "led", 128, NULL, 1, NULL);
    xTaskCreate(OledTask, "oled", 128, NULL, 1, NULL);
    xTaskCreate(CLI_Task, "cli", 512, NULL, 1, NULL);
    LOG_INFO("start task scheduler");
    vTaskStartScheduler();


    while(true) { }
}
