#ifndef motor_h
#define motor_h

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// Define motor pins
#define L_MOTOR_IN1 6  // GPIO pin for L motor input 1
#define L_MOTOR_IN2 7  // GPIO pin for L motor input 2
#define L_MOTOR_ENA 8  // GPIO pin for L motor enable
#define R_MOTOR_IN3 4  // GPIO pin for R motor input 1
#define R_MOTOR_IN4 3  // GPIO pin for R motor input 2
#define R_MOTOR_ENB 2  // GPIO pin for R motor enable

// Define encoder pins
#define L_ENCODER_POW 10 // GPIO pin for L encoder power
#define L_ENCODER_OUT 11 // GPIO pin for L encoder output
#define R_ENCODER_POW 14 // GPIO pin for R encoder power
#define R_ENCODER_OUT 15 // GPIO pin for R encoder output

// Define encoder disk specs
#define ENCODER_NOTCH 20
#define ENCODER_CIRCUMFERENCE 8.5

// Functions for motors in motor.c
void initMotorSetup();
void initMotorPWM();
void moveMotor(float pwm);
void stopMotor();
void turnMotor(int direction);

// Functions for encoders in motor.c
void getSpeed();
void encoderPulse(uint gpio, uint32_t events);
void initEncoderSetup();

#endif