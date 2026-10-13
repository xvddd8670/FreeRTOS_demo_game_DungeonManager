#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"

#include "dungeon_manager/log.h"

#define RTT_CLR_RESET   "\033[0m"
#define RTT_CLR_RED     "\033[31m"
#define RTT_CLR_GREEN   "\033[32m"
#define RTT_CLR_YELLOW  "\033[33m"
#define RTT_CLR_BLUE    "\033[34m"

static log_level_t minLevel = LOG_DBG;
static const char *const tag[] = {"ERROR    ", "WARNING  ", "INFO     ", "DEBUG    "};
static const char *const color[] = {RTT_CLR_RED, RTT_CLR_YELLOW, RTT_CLR_GREEN, RTT_CLR_BLUE};


static void Backend_Write(const char *s, unsigned n) {
    SEGGER_RTT_Write(0, s, n);
}

void Log_Init(void) {
    SEGGER_RTT_SetFlagsUpBuffer(0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);  // не ждать, если буфер полон
    setvbuf(stdout, NULL, _IONBF, 0);                               // printf без буферизации
}

void Log_SetLevel(log_level_t lvl) { minLevel = lvl; }

void Log_Write(log_level_t lvl, const char *file, int line, const char *func,
               const char *fmt, ...) {
    if (lvl > minLevel) return;

    const char *base = strrchr(file, '/');
    base = base ? base + 1 : file;

    char buf[256];
    int n = snprintf(buf, sizeof buf, "%s[%lu] %s%s:%d:%s() - ",
                     color[lvl], (unsigned long)HAL_GetTick(), tag[lvl], base, line, func);   va_list ap;
    va_start(ap, fmt);
    n += vsnprintf(buf + n, sizeof buf - n, fmt, ap);
    va_end(ap);

    const int tail = sizeof(RTT_CLR_RESET);          // 4 символа сброса + '\0'
    if (n > (int)sizeof buf - tail - 1) n = sizeof buf - tail - 1;   // место под сброс и \n

    memcpy(buf + n, RTT_CLR_RESET, sizeof(RTT_CLR_RESET) - 1);
    n += sizeof(RTT_CLR_RESET) - 1;
    buf[n++] = '\n';
    Backend_Write(buf, n);
}

int _write(int fd, char *p, int len) {
    (void)fd;
    Backend_Write(p, len);
    return len;
}
