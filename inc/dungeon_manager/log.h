#pragma once

typedef enum { LOG_ERR, LOG_WRN, LOG_INF, LOG_DBG } log_level_t;

void Log_Init(void);
void Log_SetLevel(log_level_t lvl);
// void Log_Write(log_level_t lvl, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

void Log_Write(log_level_t lvl, const char *file, int line, const char *func,
               const char *fmt, ...) __attribute__((format(printf, 5, 6)));

#define LOG_ERROR(...)   Log_Write(LOG_ERR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...) Log_Write(LOG_WRN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)    Log_Write(LOG_INF, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)   Log_Write(LOG_DBG, __FILE__, __LINE__, __func__, __VA_ARGS__)
/* перевод строки добавляется автоматически */
/*
#define LOG_ERROR(...) Log_Write(LOG_ERR, __VA_ARGS__)
#define LOG_WARNING(...) Log_Write(LOG_WRN, __VA_ARGS__)
#define LOG_INFO(...) Log_Write(LOG_INF, __VA_ARGS__)
#define LOG_DEBUG(...) Log_Write(LOG_DBG, __VA_ARGS__)
*/
