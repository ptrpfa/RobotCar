// Control L and R encoder

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "encoder.h"
#include "motor.h"

// Global variable declaration
volatile uint32_t pulseCountL = 0;
volatile uint32_t pulseCountR = 0;
uint32_t finalTimeL = 0; 
uint32_t initialTimeL = 0;
uint32_t finalTimeR = 0; 
uint32_t initialTimeR = 0;

// Function to get motor speed 
// 0 - left, 1 - right 
void getSpeed(int encoder, uint32_t pulseCount) {
    // Calculate the duration for 80 pulses in us
    uint32_t duration;

    // L encoder time calculation
    if (encoder == 0) {
        // Find total elapsed time
        duration = finalTimeL - initialTimeL;

        // Reset pulse count for L encoder
        pulseCountL = 0;
    } 
    // R encoder time calculation
    else {
        // Find total elapsed time
        duration = finalTimeR - initialTimeR;

        // Reset pulse count for R encoder
        pulseCountR = 0;
    }

    // Calculate and print motor speed in cm/s
    double distancePerHole = ENCODER_CIRCUMFERENCE / ENCODER_NOTCH;
    double speed = (distancePerHole * pulseCount) / ((double)duration / 1000000.0);
    
    // L encoder printing
    if (encoder == 0) {
        printf("L Motor Speed: %.2lf cm/s\n", speed);
    }
    // R encoder printing
    else {
        printf("R Motor Speed: %.2lf cm/s\n", speed);
    }
}

// Function to count each encoder pulse
void encoderPulse(uint gpio, uint32_t events) {
    // Get current time if there is interrupt
    uint32_t timestamp = time_us_32();

    // L encoder interrupted
    if (gpio == L_ENCODER_OUT) {
        // Increase L encoder pulse count if there is interrupt
        pulseCountL++;

        // Store initial timestamp when first L encoder pulse is received
        if (pulseCountL == 1) {
            initialTimeL = timestamp;
        }

        // Update current time
        finalTimeL = timestamp;
    } 
    // R encoder interrupted
    else {
        // Increase R encoder pulse count if there is interrupt
        pulseCountR++;

        // Store initial timestamp when first R encoder pulse is received
        if (pulseCountR == 1) {
            initialTimeR = timestamp;
        }
        
        // Update current time
        finalTimeR = timestamp;
    }

    // Check if 80 pulses are counted for the L encoder
    if (gpio == L_ENCODER_OUT && pulseCountL >= 80) {
        // Call getSpeed function when 80 pulses are counted
        getSpeed(0, pulseCountL);
    } 
    // Check if 80 pulses are counted for the R encoder
    else if (gpio == R_ENCODER_OUT && pulseCountR >= 80) {
        // Call getSpeed function when 80 pulses are counted
        getSpeed(1, pulseCountR);
    }
}

// Function to initialize pins for encoders
void initEncoderSetup() {
    // Initialize GPIO pins for L encoder
    gpio_init(L_ENCODER_POW);
    gpio_init(L_ENCODER_OUT);

    // Set GPIO pins as outputs for L encoder
    gpio_set_dir(L_ENCODER_POW, GPIO_OUT);
    gpio_set_dir(L_ENCODER_OUT, GPIO_IN);

    // Set GPIO settings for L encoder
    gpio_pull_up(L_ENCODER_OUT);
    gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &encoderPulse);

    // Initialize GPIO pins for R encoder
    gpio_init(R_ENCODER_POW);
    gpio_init(R_ENCODER_OUT);

    // Set GPIO pins as outputs for R encoder
    gpio_set_dir(R_ENCODER_POW, GPIO_OUT);
    gpio_set_dir(R_ENCODER_OUT, GPIO_IN);

    // Set GPIO settings for R encoder
    gpio_pull_up(R_ENCODER_OUT);
    gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &encoderPulse);

    // Enable the POW pins
    gpio_put(L_ENCODER_POW, 1);
    gpio_put(R_ENCODER_POW, 1);
}

int main() {
    stdio_init_all();

    // Initialise motor GPIO pins
    initMotorSetup();

    // Initialise motor PWM
    initMotorPWM();

    // Initialise encoder GPIO pins
    initEncoderSetup();

    while (1) {
        // Run at half duty cycle
        moveMotor(1563);
        sleep_ms(250);

        // Run at 32% duty cycle
        moveMotor(1000);
        sleep_ms(10000);
    }

    return 0;
}
