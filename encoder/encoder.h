#ifndef encoder_h
#define encoder_h

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Define encoder pins
#define L_ENCODER_POW 10 // GPIO pin for L encoder power
#define L_ENCODER_OUT 11 // GPIO pin for L encoder output
#define R_ENCODER_POW 17 // GPIO pin for R encoder power
#define R_ENCODER_OUT 16 // GPIO pin for R encoder output

// Define encoder disk specs
#define ENCODER_NOTCH 20
#define ENCODER_CIRCUMFERENCE 8.5
#define WHEEL_CIRCUMFERENCE 22

// External variables
extern volatile bool completeMovement;
extern volatile uint32_t oscillation;
extern volatile float actual_speed_L;
extern volatile float actual_speed_R;

// Functions for encoders
void getSpeedAndDistance(int encoder, uint32_t pulseCount);
void encoderPulse(uint gpio);
bool encoderCallback();
void initEncoderSetup();
uint32_t getGridsMoved(bool reset);
void startTracking(int targetGrids);

#endif