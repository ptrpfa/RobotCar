// Main program

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "ultrasonic.h"
#include "motor.h"

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
    /*
    stdio_init_all();
    // Sensor values
    float temperature = 0.0;
    uint64_t cm = 0.0;

    // Init pins
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    setupUltrasonicPins();

    // Run functions here

    // Temporary while loop to publish every one second
    while (true)
    {
        // Publish message

        // // Get temperature value and publish
        // temperature = read_onboard_temperature();
        // sprintf(temp_message, "%.2f", temperature);

        // // Get ultrasonic value and publish
        // cm = getCm();
        // sprintf(ultrasonic_message, "%llu", cm);
    }
    return 0;
    */

    stdio_init_all();

    // Initialise motor GPIO pins
    initMotorSetup();

    // Initialise motor PWM
    initMotorPWM();

    // Initialise ultrasonic sensor
    setupUltrasonicPins();
    kalman_state *state = kalman_init(0.125, 32, 1023, 0);

    double cm;

    while (1) {
        // Get distance from ultrasonic sensor
        for (int i = 0; i < 10; i++) {
            cm = getCm(state);
        }

        printf("Distance: %.2lf cm\n", cm);

        // If an obstacle is too close, stop motor and turn left
        if (cm < 20) {
            stopMotor();
            turnMotor(1);
        } 
        else {
            // Run the motor at half of the duty cycle
            moveMotor(1563);
        }
        
        sleep_ms(500);
    }

    return 0;
}
