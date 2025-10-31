#ifndef GM67_QR_H
#define GM67_QR_H

#include <stdbool.h>
#include <stddef.h>

void gm67_init(void);
void gm67_trigger_scan(void);
// Retorna o número de bytes lidos, ou 0 se nenhum dado estiver disponível.
int gm67_read_data(char *buffer, size_t buffer_len);

#endif // GM67_QR_H