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

typedef struct MQTT_CLIENT_T_
{
    ip_addr_t remote_addr;
    mqtt_client_t *mqtt_client;
    u32_t received;
    u32_t counter;
    u32_t reconnect;
} MQTT_CLIENT_T;

// err_t mqtt_connect(MQTT_CLIENT_T *state);
err_t mqtt_connect_to_broker(MQTT_CLIENT_T *state, mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb);

// Perform initialisation
static MQTT_CLIENT_T *mqtt_client_init(void)
{
    MQTT_CLIENT_T *state = calloc(1, sizeof(MQTT_CLIENT_T));
    if (!state)
    {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    state->received = 0;
    return state;
}

void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    MQTT_CLIENT_T *state = (MQTT_CLIENT_T *)callback_arg;
    DEBUG_printf("DNS query finished with resolved addr of %s.\n", ip4addr_ntoa(ipaddr));
    state->remote_addr = *ipaddr;
}

void run_dns_lookup(MQTT_CLIENT_T *state)
{
    DEBUG_printf("Running DNS query for %s.\n", MQTT_SERVER_HOST);

    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(MQTT_SERVER_HOST, &(state->remote_addr), dns_found, state);
    cyw43_arch_lwip_end();

    if (err == ERR_ARG)
    {
        DEBUG_printf("failed to start DNS query\n");
        return;
    }

    if (err == ERR_OK)
    {
        DEBUG_printf("no lookup needed");
        return;
    }

    while (state->remote_addr.addr == 0)
    {
        cyw43_arch_poll();
        sleep_ms(1);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if (status != 0)
    {
        DEBUG_printf("Error during connection: err %d.\n", status);
    }
    else
    {
        DEBUG_printf("MQTT connected.\n");
    }
}

err_t mqtt_publish_message(MQTT_CLIENT_T *state, const char *message, const char *topic, mqtt_request_cb_t cb)
{
    err_t err;
    u8_t qos = 0; /* 0 1 or 2, see MQTT specification.  AWS IoT does not support QoS 2 */
    u8_t retain = 0;

    cyw43_arch_lwip_begin();
    err = mqtt_publish(state->mqtt_client, topic, message, strlen(message), qos, retain, cb, state);
    cyw43_arch_lwip_end();
    if (err != ERR_OK)
    {
        DEBUG_printf("Publish err: %d\n", err);
    }

    return err;
}

err_t mqtt_subscribe_topic(MQTT_CLIENT_T *state, const char *topic, mqtt_request_cb_t cb)
{
    err_t err;

    err = mqtt_sub_unsub(state->mqtt_client, topic, 0, cb, 0, 1);
    return err;
}
u8_t mqtt_is_connected(MQTT_CLIENT_T *state)
{
    return mqtt_client_is_connected(state->mqtt_client);
}

err_t mqtt_connect_to_broker(MQTT_CLIENT_T *state, mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb)
{
    struct mqtt_connect_client_info_t ci;
    err_t err;

    // Set up new client
    state->mqtt_client = mqtt_client_new();

    state->counter = 0;

    if (state->mqtt_client == NULL)
    {
        DEBUG_printf("Failed to create new mqtt client\n");
        return -1;
    }

    memset(&ci, 0, sizeof(ci));

    ci.client_id = "PicoW";
    ci.client_user = CLIENT_USER;
    ci.client_pass = CLIENT_PASS;
    ci.keep_alive = 60;
    ci.will_topic = NULL;
    ci.will_msg = NULL;
    ci.will_retain = 0;
    ci.will_qos = 1;

    struct altcp_tls_config *tls_config;

    DEBUG_printf("Setting up TLS with cert.\n");
    ci.server_name = MQTT_SERVER_HOST;
    const char *cert = CRYPTO_CERT;

    tls_config = altcp_tls_create_config_client((const u8_t *)cert, 1 + strlen((const char *)cert));

    if (tls_config == NULL)
    {
        DEBUG_printf("Failed to initialize config\n");
        return -1;
    }

    ci.tls_config = tls_config;

    const struct mqtt_connect_client_info_t *client_info = &ci;

    err = mqtt_client_connect(state->mqtt_client, &(state->remote_addr), MQTT_SERVER_PORT, mqtt_connection_cb, state, client_info);

    mqtt_set_inpub_callback(state->mqtt_client, pub_cb, data_cb, 0);

    if (err != ERR_OK)
    {
        DEBUG_printf("mqtt_connect return %d\n", err);
    }

    return err;
}

MQTT_CLIENT_T *mqtt_setup(mqtt_incoming_publish_cb_t pub_cb, mqtt_incoming_data_cb_t data_cb)
{
    // Connect to WiFi
    if (cyw43_arch_init())
    {
        DEBUG_printf("failed to initialise\n");
        return NULL;
    }
    cyw43_arch_enable_sta_mode();

    DEBUG_printf("Connecting to WiFi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
        printf("Attempting to connect...\n");
    }

    DEBUG_printf("Connected.\n");

    // Initialise MQTT state struct
    MQTT_CLIENT_T *state = mqtt_client_init();

    // Run DNS lookup for ip address
    run_dns_lookup(state);

    // Connect to broker
    if (mqtt_connect_to_broker(state, pub_cb, data_cb) != ERR_OK)
    {
        return NULL;
    }

    return state;
}