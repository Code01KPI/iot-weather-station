#ifndef RTOS_COMMON_H
#define RTOS_COMMON_H

#include "cmsis_os.h"

#define CONFIGURE_TASK(_name, _prio, _stacksize, _func, _args)          \
    osThreadId_t _name##_th;                                            \
    StaticTask_t _name##_tcb;                                           \
    static uint32_t task_stack_##_name[_stacksize / 4];                 \
    const osThreadAttr_t task_attr_##_name = {                          \
        .stack_mem = &task_stack_##_name,                               \
        .stack_size = sizeof(task_stack_##_name),                       \
        .name = #_name,                                                 \
        .priority = (osPriority_t)_prio,                                \
        .cb_mem = &_name##_tcb,                                         \
        .cb_size = sizeof(_name##_tcb),                                 \
    };                                                                  \
    static void _func(void *argument);                                  \
    void start_##_name##_task(void)                                     \
    {                                                                   \
        _name##_th = osThreadNew(&_func, _args, &task_attr_##_name);    \
        if (_name##_th == NULL)                                         \
        {                                                               \
            while (1)                                                   \
                ;                                                       \
        }                                                               \
    }

#define CONFIGURE_MUTEX(_name)                      \
    osMutexId_t _name;                              \
    StaticSemaphore_t _name##_cb;                   \
    const osMutexAttr_t mutex_attr_##_name = {      \
        .name = #_name,                             \
        .cb_mem = &_name##_cb,                      \
        .cb_size = sizeof(_name##_cb)};             \
    void init_##_name(void)                         \
    {                                               \
        _name = osMutexNew(&mutex_attr_##_name);    \
        if (_name == NULL)                          \
        {                                           \
            while (1)                               \
                ;                                   \
        }                                           \
    }

#define CONFIGURE_SEMAPHORE(_name, _count)                              \
    osSemaphoreId_t _name;                                              \
    StaticSemaphore_t _name##_cb;                                       \
    const osSemaphoreAttr_t semaphore_attr_##_name = {                  \
        .name = #_name,                                                 \
        .cb_mem = &_name##_cb,                                          \
        .cb_size = sizeof(_name##_cb)};                                 \
    void init_##_name(void)                                             \
    {                                                                   \
        _name = osSemaphoreNew(_count, 0, &semaphore_attr_##_name);     \
        if (_name == NULL)                                              \
        {                                                               \
            while (1)                                                   \
                ;                                                       \
        }                                                               \
    }

#endif // RTOS_COMMON_H