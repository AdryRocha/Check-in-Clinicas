#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <stdbool.h>

typedef struct {
    char user_id;
    uint16_t fingerprint_id;
    char name;
} user_t;

bool storage_init(void);
bool storage_find_user_by_id(const char* id, user_t* user);
bool storage_log_checkin(const char* user_id);

#endif // DATA_STORAGE_H