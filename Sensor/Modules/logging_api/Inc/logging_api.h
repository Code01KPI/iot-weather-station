#ifndef LOGGING_API_H
#define LOGGING_API_H

#define LOG_BUFFER_SIZE 1024
#define LOG_MAX_SIZE 128

void CONSOLE_LOG(const char *format, ...);

void log_init(void);
void log_txcplt_cb(void);
void start_log_task(void);

#endif // LOGGING_API_H