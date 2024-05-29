#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "rtos_common.h"
#include "logging_api.h"

static char log_buffer[LOG_BUFFER_SIZE + 1];
static volatile uint32_t log_buffer_head = 0;
static volatile uint32_t log_buffer_tail = 0;
static volatile size_t log_buffer_size = 0;

CONFIGURE_TASK(log, osPriorityNormal, 2048 * 2, log_task, NULL);
CONFIGURE_MUTEX(log_mutex);
CONFIGURE_SEMAPHORE(log_txcplt_sem, 1);

void log_init(void)
{
    init_log_mutex();
    init_log_txcplt_sem();
}

static void log_put_char(char c)
{
    log_buffer[log_buffer_head] = c;
    log_buffer_head = (log_buffer_head + 1) % (LOG_BUFFER_SIZE + 1);
    log_buffer_size++;
}

int log_put(const char *c, size_t size)
{
    osMutexAcquire(log_mutex, osWaitForever);
    if (log_buffer_size + size + 2 > LOG_BUFFER_SIZE)
    {
        osMutexRelease(log_mutex);
        return -1; // Buffer overflow
    }

    for (size_t i = 0; i < size; ++i)
    {
        log_put_char(c[i]);
    }
    log_put_char('\r');
    log_put_char('\n');

    osMutexRelease(log_mutex);

    return 0;
}

size_t log_get_char(char *c)
{
    if (log_buffer_tail == log_buffer_head)
    {
        return 0; // Buffer is empty
    }

    osMutexAcquire(log_mutex, osWaitForever);

    *c = log_buffer[log_buffer_tail];
    log_buffer_tail = (log_buffer_tail + 1) % (LOG_BUFFER_SIZE + 1);
    log_buffer_size--;

    osMutexRelease(log_mutex);

    return 1;
}

size_t log_get(char *c, size_t size)
{
    size_t bytes_read = 0;

    osMutexAcquire(log_mutex, osWaitForever);

    while (size > 0 && log_buffer_tail != log_buffer_head)
    {
        c[bytes_read++] = log_buffer[log_buffer_tail];
        log_buffer_tail = (log_buffer_tail + 1) % (LOG_BUFFER_SIZE + 1);
        log_buffer_size--;
        size--;
    }

    osMutexRelease(log_mutex);

    return bytes_read;
}

size_t log_get_size(void)
{
    return log_buffer_size;
}

void CONSOLE_LOG(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    char temp_buffer[LOG_MAX_SIZE];
    int len = vsnprintf(temp_buffer, LOG_MAX_SIZE, format, args);

    log_put(temp_buffer, len);

    va_end(args);
}

static void log_task(void *argument)
{
    char buffer[LOG_BUFFER_SIZE + 1];
    size_t size;

    while (1)
    {
        size = log_get_size();
        if (size > 0)
        {
            buffer[LOG_BUFFER_SIZE] = '\0';
            log_get(buffer, size);
            buffer[size++] = '\0';
            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buffer, size);
            osSemaphoreAcquire(log_txcplt_sem, osWaitForever);
        }
        osDelay(5);
    }
}

void log_txcplt_cb(void)
{
    osSemaphoreRelease(log_txcplt_sem);
}
