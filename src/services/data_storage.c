#include "data_storage.h"
#include "ff.h"
#include <string.h>
#include <stdio.h>

static FATFS fs;

bool storage_init(void) {
    FRESULT fr = f_mount(&fs, "", 1);
    return fr == FR_OK;
}

bool storage_find_user_by_id(const char* id, user_t* user) {
    FIL fil;
    FRESULT fr = f_open(&fil, "users.csv", FA_READ);
    if (fr!= FR_OK) {
        return false;
    }

    char line;
    while (f_gets(line, sizeof(line), &fil)) {
        // Formato CSV esperado: user_id,fingerprint_id,name
        char read_id;
        int fp_id;
        char name;
        
        if (sscanf(line, "%63[^,],%d,%127[^\n]", read_id, &fp_id, name) == 3) {
            if (strcmp(id, read_id) == 0) {
                strcpy(user->user_id, read_id);
                user->fingerprint_id = (uint16_t)fp_id;
                strcpy(user->name, name);
                f_close(&fil);
                return true;
            }
        }
    }

    f_close(&fil);
    return false;
}

bool storage_log_checkin(const char* user_id) {
    FIL fil;
    FRESULT fr = f_open(&fil, "log.txt", FA_WRITE | FA_OPEN_APPEND);
    if (fr!= FR_OK) {
        return false;
    }
    
    // Simplesmente escreve o ID do usuário e um timestamp (simplificado)
    f_printf(&fil, "Check-in: %s\n", user_id);
    
    f_close(&fil);
    return true;
}