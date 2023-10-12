#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mqtt.h"

u32_t data_in = 0;

u8_t buffer[1025];
u8_t data_len = 0;

// Callback invoked when publish starts, contain topic and total length of payload
static void mqtt_pub_start_cb(void *arg, const char *topic, u32_t tot_len)
{
    DEBUG_printf("mqtt_pub_start_cb: topic %s\n", topic);

    if (tot_len > 1024)
    {
        DEBUG_printf("Message length exceeds buffer size, discarding");
    }
    else
    {
        data_in = tot_len;
        data_len = 0;
    }
}

// Callback for each fragment of payload that arrives
static void mqtt_pub_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    if (data_in > 0)
    {
        data_in -= len;
        memcpy(&buffer[data_len], data, len);
        data_len += len;

        if (data_in == 0)
        {
            buffer[data_len] = 0;
            DEBUG_printf("Message received: %s\n", &buffer);
        }
    }
}

// Callback to call when subscribe/unsubscribe response is received
void mqtt_sub_request_cb(void *arg, err_t err)
{
    DEBUG_printf("mqtt_sub_request_cb: err %d\n", err);
}

// Callback to call when publish is complete or has timed out
void mqtt_pub_request_cb(void *arg, err_t err)
{
    MQTT_CLIENT_T *state = (MQTT_CLIENT_T *)arg;
    DEBUG_printf("mqtt_pub_request_cb: err %d\n", err);
}

// Read temperature (temporary function)
float read_onboard_temperature()
{
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    return tempC;
}

int main()
{
    float temperature = 0.0;
    char message[128];
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    stdio_init_all();
    // Run functions here

    MQTT_CLIENT_T *state = mqtt_setup(mqtt_pub_start_cb, mqtt_pub_data_cb);

    // Subscribe to topic
    mqtt_subscribe_topic(state, "pico_w/recv", mqtt_sub_request_cb);

    absolute_time_t timeout = nil_time;

    // Temporary while loop to publish every one second
    while (true)
    {
        // Publish message
        absolute_time_t now = get_absolute_time();
        if (is_nil_time(timeout) || absolute_time_diff_us(now, timeout) <= 0)
        {
            temperature = read_onboard_temperature();
            sprintf(message, "%.2f", temperature);
            mqtt_publish_message(state, message, "pico_w/temperature", mqtt_pub_request_cb);
            timeout = make_timeout_time_ms(1000);
        }
    }
    cyw43_arch_deinit();
    return 0;
}
