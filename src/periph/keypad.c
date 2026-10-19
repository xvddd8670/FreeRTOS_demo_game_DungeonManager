#include "stm32f1xx_hal.h"
#include "dungeon_manager/periph/keypad.h"


static const uint16_t row_pins[4] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4};     // GPIOA
static const uint16_t col_pins[4] = {GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_1, GPIO_PIN_0};   // GPIOB
static const char key_map[17] = "123A456B789C*0#D";

static int last_raw = -1;
static int last_stable = -1;

void keypad_init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
                             .Mode = GPIO_MODE_OUTPUT_OD, .Speed = GPIO_SPEED_FREQ_LOW};
    HAL_GPIO_Init(GPIOA, &gpio);
    HAL_GPIO_WritePin(GPIOA, gpio.Pin, GPIO_PIN_SET);

    gpio = (GPIO_InitTypeDef){.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11,
                              .Mode = GPIO_MODE_INPUT, .Pull = GPIO_PULLUP};
    HAL_GPIO_Init(GPIOB, &gpio);
}

static int keypad_scan(void) {
    int key = -1;
    for (int r = 0; r < 4 && key < 0; r++) {
        HAL_GPIO_WritePin(GPIOA, row_pins[r], GPIO_PIN_RESET);
        for (volatile int d = 0; d < 100; d++) {}
        for (int c = 0; c < 4; c++) {
            if (HAL_GPIO_ReadPin(GPIOB, col_pins[c]) == GPIO_PIN_RESET) { key = c * 4 + r; break; }
        }
        HAL_GPIO_WritePin(GPIOA, row_pins[r], GPIO_PIN_SET);
    }
    return key;
}

char keypad_poll(void) {
    int raw = keypad_scan();
    char result = 0;
    if (raw == last_raw && raw != last_stable) {
        last_stable = raw;
        if (raw >= 0) result = key_map[raw];
    }
    last_raw = raw;
    return result;
}
