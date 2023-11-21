#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"
#include "motor.h"
#include "ultrasonic.h"

int startCar = 0;

// WIFI Credentials
const char WIFI_SSID[] = "JR";
const char WIFI_PASSWORD[] = "hello123";

int main()
{
    stdio_init_all();

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();

    // Connect to the WiFI network - loop until connected
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0)
    {
        printf("Attempting to connect...\n");
    }

    // Print a success message once connected
    printf("Connected! \n");

    // Initialise Web Server, SSI and CGI handler
    httpd_init();
    printf("Http server initialised\n");
    ssi_init();
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    // Initialise motor GPIO pins and motor PWM
    initMotorSetup();
    initMotorPWM();

    // Initialise encoder GPIO pins
    // initEncoderSetup();

    // Initialise ultrasonic sensor
    setupUltrasonicPins();
    kalman_state *state = kalman_init(1, 100, 0, 0);
    sleep_ms(1000);

    double cm;

    while (1)
    {
        if (startCar == 1)
        {
            // Get distance from ultrasonic sensor
            for (int i = 0; i < 10; i++)
            {
                cm = getCm(state);
            }

            printf("Distance: %.2lf cm\n", cm);

            // If an obstacle is too close, stop motor and turn left
            if (cm < 22)
            {
                // // Get distance from ultrasonic sensor
                // for (int i = 0; i < 10; i++) {
                //     cm = getCm(state);
                // }

                // if (cm < 10) {
                stopMotor();
                sleep_ms(1000);
                turnMotor(1);
                // }
            }
            else
            {
                // Run the motor at half of the duty cycle
                // moveMotor(1563);
                moveMotor(3125);
            }

            sleep_ms(250);
        }
        else
        {
            stopMotor();
        }
    }

    return 0;
}
