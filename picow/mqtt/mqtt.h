#ifndef MQTT_H
#define MQTT_H

#include "hardware/structs/rosc.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "crypto_consts.h"

#define DEBUG_printf printf

extern const char *cert;

typedef struct MQTT_CLIENT_T_ MQTT_CLIENT_T;

err_t mqtt_test_connect(MQTT_CLIENT_T *state);
void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void run_dns_lookup(MQTT_CLIENT_T *state);
err_t mqtt_test_publish(MQTT_CLIENT_T *state);
void mqtt_pub_request_cb(void *arg, err_t err);
void mqtt_sub_request_cb(void *arg, err_t err);
void mqtt_run_test(MQTT_CLIENT_T *state);
int mqtt_setup();

#endif
