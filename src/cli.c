#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "SEGGER_RTT.h"
#include "dungeon_manager/log.h"
#include "dungeon_manager/cli.h"

/* Заглушка: сюда позже добавить разбор команд */
static void CLI_Handle(char *line)
{
    LOGI("cmd: '%s'", line);
}

void CLI_Task(void *p)
{
    char line[64];
    uint8_t n = 0;
    char c;

    for (;;) {
        while (SEGGER_RTT_Read(0, &c, 1)) {            // канал 0 «вниз»: от хоста к МК
            if (c == '\r' || c == '\n') {
                if (n) { line[n] = 0; CLI_Handle(line); n = 0; }
            } else if (n < sizeof line - 1) {
                line[n++] = c;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
