#ifndef ultrasonic_h
#define ultrasonic_h

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIGPIN 0
#define ECHOPIN 1

typedef struct kalman_state_ kalman_state;
extern volatile bool obstacleDetected;

kalman_state *kalman_init(double q, double r, double p, double initial_value);
void get_echo_pulse(uint gpio, uint32_t events);
void kalman_update(kalman_state *state, double measurement);
void setupUltrasonicPins();
uint64_t getPulse();
double getCm(kalman_state *state);

#endif