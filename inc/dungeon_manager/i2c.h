#pragma once
#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

void I2C1_Init(void);
HAL_StatusTypeDef I2C1_WriteDMA(uint16_t addr, uint8_t *buf, uint16_t len);
HAL_StatusTypeDef I2C1_MemWriteDMA(uint16_t addr, uint16_t mem, uint16_t mem_size, uint8_t *buf, uint16_t len);
HAL_StatusTypeDef I2C1_MemReadDMA(uint16_t addr, uint16_t mem, uint16_t mem_size, uint8_t *buf, uint16_t len);
HAL_StatusTypeDef I2C1_IsReady(uint16_t addr);
