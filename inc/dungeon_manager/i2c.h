#pragma once
#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

void I2C1_Init(void);
HAL_StatusTypeDef I2C1_WriteDMA(uint16_t addr, uint8_t *buf, uint16_t len);
