#ifndef ultrasonic_h
#define ultrasonic_h

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIGPIN 0
#define ECHOPIN 1

typedef struct kalman_state_ kalman_state;

static kalman_state *kalman_init(double q, double r, double p, double initial_value);
void kalman_update(kalman_state *state, double measurement);
void setupUltrasonicPins();
uint64_t getPulse();
double getCm(kalman_state *state);

#endif