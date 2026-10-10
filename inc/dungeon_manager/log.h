#pragma once

typedef enum { LOG_ERR, LOG_WRN, LOG_INF, LOG_DBG } log_level_t;

void Log_Init(void);
void Log_SetLevel(log_level_t lvl);
void Log_Write(log_level_t lvl, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

/* перевод строки добавляется автоматически */
#define LOGE(...) Log_Write(LOG_ERR, __VA_ARGS__)
#define LOGW(...) Log_Write(LOG_WRN, __VA_ARGS__)
#define LOGI(...) Log_Write(LOG_INF, __VA_ARGS__)
#define LOGD(...) Log_Write(LOG_DBG, __VA_ARGS__)
