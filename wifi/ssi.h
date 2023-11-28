#ifndef ssi_h
#define ssi_h

#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

// SSI tags - tag length limited to 8 bytes by default
extern const char *ssi_tags[];

extern bool isMazeMapped;

// Functions for SSI handler
void ssi_init();
u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen);

#endif
