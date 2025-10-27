#include "event_handler.h"
#include "pico/stdlib.h"
#include "pico/critical_section.h"
#include <string.h>

#define EVENT_QUEUE_SIZE 16

static event_t event_queue;
static volatile uint8_t queue_head = 0;
static volatile uint8_t queue_tail = 0;

static critical_section_t queue_crit_sec;

void event_handler_init(void) {
    critical_section_init(&queue_crit_sec);
}

void event_post(const event_t *event) {
    critical_section_enter_blocking(&queue_crit_sec);
    
    uint8_t next_head = (queue_head + 1) % EVENT_QUEUE_SIZE;
    if (next_head!= queue_tail) {
        event_queue[queue_head] = *event;
        queue_head = next_head;
    }
    // else: a fila está cheia, o evento é descartado.
    // Em uma aplicação real, um log de erro seria apropriado aqui.
    
    critical_section_exit(&queue_crit_sec);
}

bool event_get(event_t *event) {
    critical_section_enter_blocking(&queue_crit_sec);
    
    if (queue_head == queue_tail) {
        critical_section_exit(&queue_crit_sec);
        return false; // Fila vazia
    }
    
    *event = event_queue[queue_tail];
    queue_tail = (queue_tail + 1) % EVENT_QUEUE_SIZE;
    
    critical_section_exit(&queue_crit_sec);
    return true;
}