#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"
// #include "log.h"

static void ringbuf_put_byte(struct ring_buffer *r, uint8_t byte)
{
    uint8_t *tmp_head = r->head;
    *tmp_head++ = byte;
    if (tmp_head >= r->buff + r->size)
    {
        tmp_head = r->buff;
    }
    r->head = tmp_head;
}

size_t ringbuf_get_size(struct ring_buffer *r)
{
    osMutexAcquire(*r->mutex, osWaitForever);
    if (r->head == r->tail)
    {
        if (r->is_full)
        {
            osMutexRelease(*r->mutex);
            return r->size;
        }
        osMutexRelease(*r->mutex);
        return 0;
    }

    size_t ret;
    if (r->head > r->tail)
    {
        ret = (size_t)(r->head - r->tail);
    }
    else
    {
        ret = r->size - (size_t)(r->head - r->tail);
    }
    osMutexRelease(*r->mutex);
    return ret;
}

size_t ringbuf_get_msg_size(struct ring_buffer *r)
{
    if (ringbuf_get_size(r) == 0)
    {
        return 0; // Buffer overflow
    }
    size_t sz;
    osMutexAcquire(*r->mutex, osWaitForever);
    sz = *r->tail;
    osMutexRelease(*r->mutex);

    return sz;
}

int ringbuf_put(struct ring_buffer *r, uint8_t *in_data, size_t size)
{
    if (r->size - ringbuf_get_size(r) + 1 < size)
    {
        return -1; // Buffer overflow
    }

    osMutexAcquire(*r->mutex, osWaitForever);
    ringbuf_put_byte(r, (uint8_t)size);
    for (size_t i = 0; i < size; ++i)
    {
        ringbuf_put_byte(r, in_data[i]);
    }

    if (r->head == r->tail)
    {
        r->is_full = 1;
    }
    osMutexRelease(*r->mutex);

    return 0;
}

int ringbuf_get(struct ring_buffer *r, uint8_t *out_data, size_t *size)
{
    if (*size <= 0)
    {
        return -1;
    }

    size_t bytes_read = 0;
    size_t tmp_size = 0;
    uint8_t *tmp_tail;

    tmp_size = ringbuf_get_size(r);
    if (tmp_size == 0)
    {
        return -1;
    }

    osMutexAcquire(*r->mutex, osWaitForever);
    tmp_tail = r->tail;
    tmp_size = *tmp_tail++;
    if (tmp_tail >= r->buff + r->size)
    {
        tmp_tail = r->buff;
    }

    if (tmp_size <= *size)
    {
        for (size_t i = 0; i < tmp_size; i++)
        {
            out_data[bytes_read++] = *tmp_tail++;
            if (tmp_tail >= r->buff + r->size)
            {
                tmp_tail = r->buff;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < *size - 1; i++)
        {
            out_data[bytes_read++] = *tmp_tail++;
            if (tmp_tail >= r->buff + r->size)
            {
                tmp_tail = r->buff;
            }
        }
        out_data[bytes_read++] = *tmp_tail;
        *tmp_tail = (uint8_t)(tmp_size - *size);
    }

    r->is_full = 0;
    r->tail = tmp_tail;
    *size = bytes_read;
    osMutexRelease(*r->mutex);

    return 0;
}

void ringbuf_clear(struct ring_buffer *r)
{
    osMutexAcquire(*r->mutex, osWaitForever);
    r->head = r->buff;
    r->tail = r->buff;
    r->is_full = 0;
    osMutexRelease(*r->mutex);
}
