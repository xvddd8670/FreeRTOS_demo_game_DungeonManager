#include <stdarg.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include "dungeon_manager/log.h"

static log_level_t minLevel = LOG_DBG;
static const char tag[] = {'E', 'W', 'I', 'D'};

/* Единственное место, которое знает про RTT.
   Для другого вывода (UART, дисплей) меняется только эта функция. */
static void Backend_Write(const char *s, unsigned n)
{
    SEGGER_RTT_Write(0, s, n);
}

void Log_Init(void)
{
    SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);  // не ждать, если буфер полон
    setvbuf(stdout, NULL, _IONBF, 0);                               // printf без буферизации
}

void Log_SetLevel(log_level_t lvl) { minLevel = lvl; }

void Log_Write(log_level_t lvl, const char *fmt, ...)
{
    if (lvl > minLevel) return;

    char buf[128];
    int n = snprintf(buf, sizeof buf, "[%lu] %c ", (unsigned long)HAL_GetTick(), tag[lvl]);

    va_list ap;
    va_start(ap, fmt);
    n += vsnprintf(buf + n, sizeof buf - n, fmt, ap);
    va_end(ap);

    if (n > (int)sizeof buf - 2) n = sizeof buf - 2;   // обрезано — оставляем место под \n
    buf[n++] = '\n';
    Backend_Write(buf, n);
}

/* Старые printf() тоже идут в RTT */
int _write(int fd, char *p, int len)
{
    (void)fd;
    Backend_Write(p, len);
    return len;
}
