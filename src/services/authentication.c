#include "services/authentication.h"
#include "data_storage.h"
#include "r307s_fingerprint.h"
#include <stdio.h>

static user_t current_user;
static bool user_found = false;

bool auth_start_for_qr(const char* qr_data) {
    user_found = storage_find_user_by_id(qr_data, &current_user);
    if (user_found) {
        printf("User found: %s, FP ID: %d\n", current_user.name, current_user.fingerprint_id);
    } else {
        printf("User not found for QR: %s\n", qr_data);
    }
    return user_found;
}

bool auth_check_fingerprint(void) {
    if (!user_found) {
        return false;
    }

    if (r307s_get_image()!= R307S_OK) {
        return false;
    }
    if (r307s_image_to_tz(1)!= R307S_OK) {
        return false;
    }

    uint16_t found_finger_id;
    uint16_t score;
    if (r307s_search_finger(&found_finger_id, &score) == R307S_OK) {
        if (found_finger_id == current_user.fingerprint_id) {
            storage_log_checkin(current_user.user_id);
            return true;
        }
    }
    
    return false;
}