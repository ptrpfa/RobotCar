#ifndef ultrasonic_h
#define ultrasonic_h

#define TRIGPIN 0
#define ECHOPIN 1

void setupUltrasonicPins();
uint64_t getCm();
#endif