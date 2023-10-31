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

// Functions for motors
void initMotorSetup();
void initMotorPWM();
void moveMotor(float pwm);
void stopMotor();
void turnMotor(int direction);

#endif