#ifndef NETWORK_SYNC_H
#define NETWORK_SYNC_H

#include <stdbool.h>

// Estrutura de exemplo
typedef struct {
    char user_id;
    uint64_t timestamp;
} checkin_record_t;

bool network_sync_checkin_record(const checkin_record_t* record);

#endif // NETWORK_SYNC_H