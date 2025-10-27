#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stdbool.h>

bool auth_start_for_qr(const char* qr_data);
bool auth_check_fingerprint(void);

#endif // AUTHENTICATION_H