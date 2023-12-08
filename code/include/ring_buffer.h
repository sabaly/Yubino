#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

struct ring_buffer {
    uint8_t *buffer;
    uint8_t head;
    uint8_t tail;
    uint8_t maxlen;
};

void ring_buffer__init(struct ring_buffer *, uint8_t *, uint8_t);
void ring_buffer__push(struct ring_buffer *, uint8_t);
uint8_t ring_buffer__pop(struct ring_buffer *, uint8_t *);

#endif