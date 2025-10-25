#include "data_storage.h"
#include "sdcard_spi.h"
#include "ff.h"
#include <stdio.h>

static FATFS fs;

// Formato do header do arquivo de agendamentos 
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint32_t timestamp;
    uint16_t num_records;
} data_header_t;

#define APPOINTMENTS_FILE "0:/appointments.dat"
#define CHECKINS_FILE "0:/checkins.log"
#define DATA_FILE_MAGIC 0xDEADBEEF

bool storage_init() {
    return sd_card_init(&fs);
}

bool storage_load_daily_appointments(appointment_record_t *records, uint16_t *count) {
    FIL fil;
    FRESULT fr = f_open(&fil, APPOINTMENTS_FILE, FA_READ);
    if (fr!= FR_OK) {
        return false;
    }

    data_header_t header;
    UINT br;
    fr = f_read(&fil, &header, sizeof(header), &br);
    if (fr!= FR_OK |

| br!= sizeof(header) |
| header.magic!= DATA_FILE_MAGIC) {
        f_close(&fil);
        return false;
    }

    if (header.num_records > MAX_APPOINTMENTS) {
        *count = MAX_APPOINTMENTS;
    } else {
        *count = header.num_records;
    }

    fr = f_read(&fil, records, sizeof(appointment_record_t) * (*count), &br);
    f_close(&fil);

    return (fr == FR_OK);
}

bool storage_log_checkin(const appointment_record_t *record) {
    FIL fil;
    FRESULT fr = f_open(&fil, CHECKINS_FILE, FA_WRITE | FA_OPEN_APPEND);
    if (fr!= FR_OK) {
        return false;
    }

    // Formata a linha de log
    char log_buffer;
    // A implementação de `sync_get_network_time()` forneceria o tempo atual.
    // Por enquanto, usamos o tempo do agendamento.
    snprintf(log_buffer, sizeof(log_buffer), "CPF: %s, CRM: %s, CheckInTime: %u\n",
             record->cpf, record->crm, (unsigned int)record->appointment_time);

    UINT bw;
    f_write(&fil, log_buffer, strlen(log_buffer), &bw);
    f_close(&fil);

    return (fr == FR_OK);
}