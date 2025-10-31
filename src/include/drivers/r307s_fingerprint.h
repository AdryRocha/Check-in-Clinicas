#ifndef R307S_FINGERPRINT_H
#define R307S_FINGERPRINT_H

#include <stdint.h>
#include <stdbool.h>

// Códigos de confirmação do sensor
#define R307S_OK                    0x00
#define R307S_ERROR_COMM            0x01
#define R307S_NO_FINGER             0x02
#define R307S_IMAGE_FAIL            0x03
#define R307S_IMAGE_MESS            0x06
#define R307S_FEATURE_FAIL          0x07
#define R307S_NO_MATCH              0x08
#define R307S_NOT_FOUND             0x09
#define R307S_ENROLL_FAIL           0x0A
#define R307S_BAD_LOCATION          0x0B
#define R307S_DB_RANGE_FAIL         0x0C
#define R307S_UPLOAD_FEATURE_FAIL   0x0D
#define R307S_PACKET_RESPONSE_FAIL  0x0E
#define R307S_UPLOAD_IMAGE_FAIL     0x0F
#define R307S_DELETE_FAIL           0x10
#define R307S_DB_CLEAR_FAIL         0x11
#define R307S_PASS_FAIL             0x13
#define R307S_INVALID_IMAGE         0x15
#define R307S_FLASH_ERR             0x18
#define R307S_INVALID_REG           0x1A
#define R307S_ADDR_CODE             0x20
#define R307S_PASS_VERIFY           0x21

void r307s_init(void);
bool r307s_verify_password(void);
uint8_t r307s_get_image(void);
uint8_t r307s_image_to_tz(uint8_t slot);
uint8_t r307s_search_finger(uint16_t *finger_id, uint16_t *score);

#endif // R307S_FINGERPRINT_H