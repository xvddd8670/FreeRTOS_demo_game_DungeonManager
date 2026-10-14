#include <stdio.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "dungeon_manager/periph/i2c.h"
#include "dungeon_manager/utils/log.h"

static DMA_HandleTypeDef hdma_i2c1_tx;
static DMA_HandleTypeDef hdma_i2c1_rx;
static SemaphoreHandle_t i2c_mx;
static SemaphoreHandle_t i2c_done;

I2C_HandleTypeDef hi2c1;


void I2C1_Init(void) {
    i2c_done = xSemaphoreCreateBinary();
    i2c_mx = xSemaphoreCreateMutex();

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
        printf("i2c init fail\n");
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance != I2C1) return;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef g = {
        .Pin   = GPIO_PIN_6 | GPIO_PIN_7,
        .Mode  = GPIO_MODE_AF_OD,
        .Pull  = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH,
    };
    HAL_GPIO_Init(GPIOB, &g);

    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_i2c1_tx.Instance                 = DMA1_Channel6;
    hdma_i2c1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode                = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_i2c1_tx);
    __HAL_LINKDMA(hi2c, hdmatx, hdma_i2c1_tx);

    hdma_i2c1_rx.Instance                 = DMA1_Channel7;
    hdma_i2c1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_i2c1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_i2c1_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.Mode                = DMA_NORMAL;
    hdma_i2c1_rx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_i2c1_rx);
    __HAL_LINKDMA(hi2c, hdmarx, hdma_i2c1_rx);

    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void DMA1_Channel6_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_i2c1_tx); }
void DMA1_Channel7_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_i2c1_rx); }
void I2C1_EV_IRQHandler(void)       { HAL_I2C_EV_IRQHandler(&hi2c1); }
void I2C1_ER_IRQHandler(void)       { HAL_I2C_ER_IRQHandler(&hi2c1); }

static void GiveFromISR(void) {
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(i2c_done, &woken);
    portYIELD_FROM_ISR(woken);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *h) { GiveFromISR(); }
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *h)        { GiveFromISR(); }
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *h) { GiveFromISR(); }
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *h) { GiveFromISR(); }

static HAL_StatusTypeDef wait_done(HAL_StatusTypeDef start) {
    if (start != HAL_OK) {
        printf("i2c start fail=%d\n", start);
        return HAL_ERROR;
    }

    BaseType_t got = xSemaphoreTake(i2c_done, pdMS_TO_TICKS(100));
    uint32_t err = hi2c1.ErrorCode;

    if (got != pdTRUE) {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_Init(&hi2c1);
        return HAL_TIMEOUT;
    }
    return (err == 0) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef I2C1_MemWriteDMA(uint16_t addr, uint16_t mem, uint16_t mem_size, uint8_t *buf, uint16_t len) {
    xSemaphoreTake(i2c_mx, portMAX_DELAY);
    xSemaphoreTake(i2c_done, 0);
    HAL_StatusTypeDef res = wait_done(HAL_I2C_Mem_Write_DMA(&hi2c1, addr, mem, mem_size, buf, len));
    xSemaphoreGive(i2c_mx);
    return res;
}

HAL_StatusTypeDef I2C1_MemReadDMA(uint16_t addr, uint16_t mem, uint16_t mem_size, uint8_t *buf, uint16_t len) {
    xSemaphoreTake(i2c_mx, portMAX_DELAY);
    xSemaphoreTake(i2c_done, 0);
    HAL_StatusTypeDef res = wait_done(HAL_I2C_Mem_Read_DMA(&hi2c1, addr, mem, mem_size, buf, len));
    xSemaphoreGive(i2c_mx);
    return res;
}

HAL_StatusTypeDef I2C1_IsReady(uint16_t addr) {
    xSemaphoreTake(i2c_mx, portMAX_DELAY);
    HAL_StatusTypeDef res = HAL_I2C_IsDeviceReady(&hi2c1, addr, 1, 5);
    xSemaphoreGive(i2c_mx);
    return res;
}

HAL_StatusTypeDef I2C1_WriteDMA(uint16_t addr, uint8_t *buf, uint16_t len) {
    xSemaphoreTake(i2c_mx, portMAX_DELAY);
    xSemaphoreTake(i2c_done, 0);
    HAL_StatusTypeDef res = wait_done(HAL_I2C_Master_Transmit_DMA(&hi2c1, addr, buf, len));
    xSemaphoreGive(i2c_mx);
    return res;
}
