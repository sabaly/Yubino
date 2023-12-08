#include <stdint.h>
#include <util/atomic.h>

#include "ring_buffer.h"

void
ring_buffer__init(struct ring_buffer *rb, uint8_t *buffer, uint8_t buffer_size)
{
    rb->buffer = buffer;
    rb->maxlen = buffer_size;
    rb->head = 0;
    rb->tail = 0;
}

uint8_t
ring_buffer__pop(struct ring_buffer *rb, uint8_t *data)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t next;

        if (rb->head == rb->tail) {  // if the head == tail, we don't have any data
            return 1;
        }

        next = rb->head + 1;  // next is where head will point to after this read.
        if(next >= rb->maxlen) {
            next = 0;
        }

        *data = rb->buffer[rb->head];  // Read data and then move
        rb->head = next;              // tail to next offset.
    }
    return 0;  // return success to indicate successful push.
}


void
ring_buffer__push(struct ring_buffer *rb, uint8_t data)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t next;

        next = rb->tail + 1;  // next is where tail will point to after this write.
        if (next >= rb->maxlen) {
            next = 0;
        }

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            if (next == rb->head) { // if the tail + 1 == head, circular buffer is full
                uint8_t next_head = rb->head + 1;
                if (next_head >= rb->maxlen) {
                    next_head = 0;
                }
                rb->head = next_head;
            }
        }

        rb->buffer[rb->tail] = data;  // Load data and then move
        rb->tail = next;             // head to next data offset.
    }
}