#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "rtos_common.h"

struct ring_buffer
{
    uint8_t *buff;
    size_t size;
    uint8_t *head;
    uint8_t *tail;
    uint8_t is_full;
    osMutexId_t *mutex;
};

#define CREATE_RINGBUF(_name, _size)    \
    CONFIGURE_MUTEX(_name##_mutex);     \
    uint8_t _name##_buffer[_size];      \
    struct ring_buffer _name = {        \
        .buff = _name##_buffer,         \
        .size = _size,                  \
        .head = _name##_buffer,         \
        .tail = _name##_buffer,         \
        .is_full = 0,                   \
        .mutex = &_name##_mutex,        \
    };                                  \
    void init_##_name(void)             \
    {                                   \
        init_##_name##_mutex();         \
    }

size_t ringbuf_get_size(struct ring_buffer *r);
size_t ringbuf_get_msg_size(struct ring_buffer *r);
int ringbuf_put(struct ring_buffer *r, uint8_t *in_data, size_t size);
int ringbuf_get(struct ring_buffer *r, uint8_t *out_data, size_t *size);
void ringbuf_clear(struct ring_buffer *r);

#endif // RING_BUFFER_H