#ifndef wallsensor_H
#define wallsensor_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

extern volatile bool wallDetected;

// Macros for pin configurations
#define LEFT_IR_PIN 27
#define RIGHT_IR_PIN 28

// Functions for wall sensors
void wall_detected(uint gpio, uint32_t events);
void init_wallsensors();

#endif