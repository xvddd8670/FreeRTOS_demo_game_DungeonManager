#pragma once
#include <stdint.h>
extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION                     1
#define configCPU_CLOCK_HZ                       (SystemCoreClock)
#define configTICK_RATE_HZ                       1000
#define configMAX_PRIORITIES                     5
#define configMINIMAL_STACK_SIZE                 128      /* в словах (4 байта) */
#define configTOTAL_HEAP_SIZE                    (8 * 1024)
#define configUSE_16_BIT_TICKS                   0
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configMAX_TASK_NAME_LEN                  16
#define configUSE_TRACE_FACILITY                 0
#define configIDLE_SHOULD_YIELD                  1
#define configUSE_MALLOC_FAILED_HOOK             1
#define configCHECK_FOR_STACK_OVERFLOW           2

/* У F103 4 бита приоритетов прерываний */
#define configKERNEL_INTERRUPT_PRIORITY          (15 << 4)
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     (5 << 4)

#define INCLUDE_vTaskDelay                       1
#define INCLUDE_xTaskGetSchedulerState            1

/* Подменяем имена обработчиков ядра на стандартные имена из startup-файла */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
