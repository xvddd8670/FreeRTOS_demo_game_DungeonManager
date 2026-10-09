#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "ssd1306.h"

extern void initialise_monitor_handles(void);

void SysTick_Handler(void) { HAL_IncTick(); }

I2C_HandleTypeDef hi2c1;

static void MX_I2C1_Init(void)
{
    printf("i2c init start\n");
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;              // 100kHz, стандартный режим
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;                  // главное устройство, собственный адрес не важен
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        // при необходимости добавьте сюда обработку ошибки, например, включите светодиод
    }
    printf("i2c init finish\n");
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(hi2c->Instance == I2C1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();      // I2C1 на STM32F1 обычно использует PB6/PB7
        __HAL_RCC_I2C1_CLK_ENABLE();

        // PB6 -> I2C1_SCL, PB7 -> I2C1_SDA
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;     // альтернативная функция, открытый сток
        GPIO_InitStruct.Pull = GPIO_PULLUP;          // подтягивающий резистор (если на внешней схеме уже есть подтяжка, можно установить NOPULL)
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

// -------------------------
// oled
// -------------------------
#define OLED_ADDR 0x78

static void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = 0x00; // Управляющий байт: команда
    buf[1] = cmd;  // Сама команда
    
    // Адрес 0x78 (0x3C << 1), отправляем 2 байта
    /*
    if (HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 100) != HAL_OK){
        printf("cmd fail %02X\n", cmd);
    }
    */

    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 100);
    if (st != HAL_OK)
        printf("cmd %02X st=%d err=0x%lX\n", cmd, st, hi2c1.ErrorCode);
}

/* Отправка данных (пикселей) в OLED */
static void OLED_WriteData(uint8_t data)
{
    uint8_t buf[2];
    buf[0] = 0x40; // Управляющий байт: данные
    buf[1] = data; // Сами данные
    
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 2, 100);
    //HAL_I2C_Mem_Write(&hi2c1, OLED_ADDR, 0x40, 1, &data, 1, 100);
}

static void OLED_Init(void)
{
    printf("oled init\n");
    HAL_Delay(100); // Ждем стабилизации питания

    OLED_WriteCmd(0xAE); // Выключить дисплей
    
    OLED_WriteCmd(0xD5); // Настройка частоты
    OLED_WriteCmd(0x80);
    
    OLED_WriteCmd(0xA8); // Настройка MUX (строк)
    OLED_WriteCmd(0x3F); // 64 строки для 128x64
    
    OLED_WriteCmd(0xD3); // Смещение дисплея
    OLED_WriteCmd(0x00);
    
    OLED_WriteCmd(0x40); // Начальная строка = 0
    
    OLED_WriteCmd(0x8D); // Включение charge pump
    OLED_WriteCmd(0x14); // Внутренний VCC (стандарт для модулей)
    
    OLED_WriteCmd(0x20); // Режим адресации памяти
    //OLED_WriteCmd(0x00); // Горизонтальный режим (удобно для заполнения экрана)
    OLED_WriteCmd(0x02);
    
    OLED_WriteCmd(0xA1); // Ремап сегментов (влево/вправо)
    OLED_WriteCmd(0xC8); // Направление сканирования COM
    
    OLED_WriteCmd(0xDA); // Настройка COM pins
    OLED_WriteCmd(0x12); // Для 128x64
    
    OLED_WriteCmd(0x81); // Контраст
    OLED_WriteCmd(0x7F); // Стандартное значение
    
    OLED_WriteCmd(0xD9); // Предзаряд
    OLED_WriteCmd(0xF1);
    
    OLED_WriteCmd(0xDB); // VCOMH
    OLED_WriteCmd(0x40);
    
    OLED_WriteCmd(0xA4); // Возобновить отображение
    OLED_WriteCmd(0xA6); // Нормальный (не инвертированный) режим
    
    OLED_WriteCmd(0xAF); // **Включить дисплей**
}
static uint8_t fb[128 * 8];   // 1024 байта: 8 страниц по 128 колонок

void OLED_Pixel(uint8_t x, uint8_t y, uint8_t on)
{
    if (x >= 128 || y >= 64) return;
    if (on) fb[x + (y / 8) * 128] |=  (1 << (y % 8));
    else    fb[x + (y / 8) * 128] &= ~(1 << (y % 8));
}

void OLED_Update(void)
{
    for (uint8_t page = 0; page < 8; page++) {
        OLED_WriteCmd(0xB0 + page);  // выбрать страницу
        OLED_WriteCmd(0x00);         // колонка 0 (младшие 4 бита)
        OLED_WriteCmd(0x10);         // колонка 0 (старшие 4 бита)

        uint8_t buf[129];
        buf[0] = 0x40;               // control byte: данные
        memcpy(&buf[1], &fb[page * 128], 128);
        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, buf, 129, 100);
    }
}

// --------------------------------
// main
// --------------------------------
int main(void) {

    initialise_monitor_handles();
    printf("start main\n");

    HAL_Init();

    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef g = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_LOW};


    HAL_GPIO_Init(GPIOC, &g);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    MX_I2C1_Init();

    HAL_Delay(100);


    OLED_Init();
    memset(fb, 0, sizeof(fb));      // очистить
    for (uint8_t x = 0; x < 128; x++) OLED_Pixel(x, 32, 1);  // горизонтальная линия
    OLED_Pixel(10, 10, 1);
    OLED_Update();                  // вывести на экран


    printf("run main while\n");


    while(true) { 
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); HAL_Delay(1250);
    }
}
