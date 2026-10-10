#include <stdio.h>
#include "dungeon_manager/i2c.h"

#include "FreeRTOS.h"
#include "semphr.h"

static DMA_HandleTypeDef hdma_i2c1_tx;
static SemaphoreHandle_t i2cDone;

I2C_HandleTypeDef hi2c1;

void I2C1_Init(void)
{
    i2cDone = xSemaphoreCreateBinary();

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;            // 100 кГц; для 400 кГц: 400000 + I2C_DUTYCYCLE_2
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

/* HAL вызывает сам из HAL_I2C_Init */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance != I2C1) return;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    GPIO_InitTypeDef g = {
        .Pin   = GPIO_PIN_6 | GPIO_PIN_7,      // PB6 = SCL, PB7 = SDA
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

    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void DMA1_Channel6_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_i2c1_tx); }
void I2C1_EV_IRQHandler(void)       { HAL_I2C_EV_IRQHandler(&hi2c1); }
void I2C1_ER_IRQHandler(void)       { HAL_I2C_ER_IRQHandler(&hi2c1); }

static void GiveFromISR(void)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(i2cDone, &woken);
    portYIELD_FROM_ISR(woken);
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *h) { GiveFromISR(); }
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *h)        { GiveFromISR(); }

HAL_StatusTypeDef I2C1_WriteDMA(uint16_t addr, uint8_t *buf, uint16_t len)
{
    if (HAL_I2C_Master_Transmit_DMA(&hi2c1, addr, buf, len) != HAL_OK) return HAL_ERROR;
    if (xSemaphoreTake(i2cDone, pdMS_TO_TICKS(100)) != pdTRUE)         return HAL_TIMEOUT;
    return hi2c1.ErrorCode ? HAL_ERROR : HAL_OK;
}
