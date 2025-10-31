// event_handler.c (IMPLEMENTAÇÃO NECESSÁRIA)
#include "core/event_handler.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include <string.h>

#define EVENT_QUEUE_SIZE 16

typedef struct {
    event_t events[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    mutex_t mutex;
} event_queue_t;

static event_queue_t event_queue;

void event_handler_init(void) {
    event_queue.head = 0;
    event_queue.tail = 0;
    mutex_init(&event_queue.mutex);
}

void event_post(const event_t *event) {
    mutex_enter_blocking(&event_queue.mutex);
    
    uint8_t next_head = (event_queue.head + 1) % EVENT_QUEUE_SIZE;
    
    if (next_head != event_queue.tail) {
        memcpy(&event_queue.events[event_queue.head], event, sizeof(event_t));
        event_queue.head = next_head;
    }
    
    mutex_exit(&event_queue.mutex);
}

bool event_get(event_t *event) {
    bool has_event = false;
    
    mutex_enter_blocking(&event_queue.mutex);
    
    if (event_queue.tail != event_queue.head) {
        memcpy(event, &event_queue.events[event_queue.tail], sizeof(event_t));
        event_queue.tail = (event_queue.tail + 1) % EVENT_QUEUE_SIZE;
        has_event = true;
    }
    
    mutex_exit(&event_queue.mutex);
    
    return has_event;
}