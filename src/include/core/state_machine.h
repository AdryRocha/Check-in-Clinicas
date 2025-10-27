#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "event_handler.h"

typedef enum {
    STATE_INIT,
    STATE_IDLE,
    STATE_AWAITING_QR,
    STATE_AWAITING_FINGERPRINT,
    STATE_AUTHENTICATING,
    STATE_CHECKIN_SUCCESS,
    STATE_CHECKIN_FAILURE,
    STATE_ERROR
} system_state_t;

void state_machine_init(void);
void state_machine_run(void);

#endif // STATE_MACHINE_H