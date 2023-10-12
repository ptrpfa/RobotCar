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

void run_dns_lookup(MQTT_CLIENT_T *state);
void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
err_t mqtt_publish_message(MQTT_CLIENT_T *state, const char *message, const char *topic, mqtt_request_cb_t cb);
err_t mqtt_subscribe_topic(MQTT_CLIENT_T *state, const char *topic, mqtt_request_cb_t cb);
err_t mqtt_connect_to_broker(MQTT_CLIENT_T *state, mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb);
MQTT_CLIENT_T *mqtt_setup(mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb);

#endif
