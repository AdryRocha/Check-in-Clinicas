#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <stdint.h>

typedef enum {
    EV_NONE,
    EV_SYSTEM_START,
    EV_QR_CODE_SCANNED,
    EV_FINGER_DETECTED,
    EV_AUTH_SUCCESS,
    EV_AUTH_FAILURE,
    EV_TIMEOUT,
    // Adicionar mais eventos conforme necessário
} event_type_t;

typedef struct {
    event_type_t type;
    union {
        char qr_data;
        uint16_t finger_id;
    } payload;
} event_t;

void event_handler_init(void);
void event_post(const event_t *event);
bool event_get(event_t *event);

#endif // EVENT_HANDLER_H