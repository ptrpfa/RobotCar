#ifndef cgi_h
#define cgi_h

#include "pico/cyw43_arch.h"
#include "lwip/apps/httpd.h"

// Global variable for start flag
extern int startCar;

// Functions for CGI handler
void cgi_init(void);
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char * cgi_engine_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

#endif
