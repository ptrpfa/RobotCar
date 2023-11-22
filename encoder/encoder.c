// Control L and R encoder

#include "encoder.h"
// #include "motor.h"

// Global variable declaration
volatile bool movedOneGrid = false;
uint32_t pulseCountL = 0;
uint32_t pulseCountR = 0;
double movedDistance = 0.0;
double totalDistanceL = 0.0;
double totalDistanceR = 0.0;

// Function to get motor speed and distance
void getSpeedAndDistance(int encoder, uint32_t pulseCount, double *totalDistance) {
    // Calculate motor speed in cm/s
    double distancePerHole = ENCODER_CIRCUMFERENCE / ENCODER_NOTCH;
    double speed = (distancePerHole * pulseCount) / 1.0;

    // Calculate and accumulate the distance
    double distance = speed * 1.0;
    *totalDistance += distance;
    movedDistance += distance;

    // Print motor speed and total distance
    if (encoder == 0) {
        printf("L Motor Speed: %.2lf cm/s, L Total Distance: %.2lf cm\n", speed, *totalDistance);
    } 
    else {
        printf("R Motor Speed: %.2lf cm/s, R Total Distance: %.2lf cm\n", speed, *totalDistance);
    }

    // If car has moved at least 1 grid (17cm), trigger flag
    if (movedDistance >= 17) {
        printf("A GRID PASSED\n");

        // Trigger 1 grid moved flag
        movedOneGrid = true;

        // Reset the distance moved
        movedDistance = 0.0;
    }
}

// Function to count each encoder's pulse
void encoderPulse(uint gpio) {
    // L encoder interrupted
    if (gpio == L_ENCODER_OUT) {
        pulseCountL++;
    } 
    // R encoder interrupted
    else if (gpio == R_ENCODER_OUT) {
        pulseCountR++;
    }
}

// Function to interrupt every second
bool encoderCallback(struct repeating_timer *t) {
    // Call getSpeedAndDistance function every second
    getSpeedAndDistance(0, pulseCountL, &totalDistanceL);
    getSpeedAndDistance(1, pulseCountR, &totalDistanceR);

    // Reset the pulse counts
    pulseCountL = 0;
    pulseCountR = 0;

    return true;
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
    // gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &encoderPulse);

    // Initialize GPIO pins for R encoder
    gpio_init(R_ENCODER_POW);
    gpio_init(R_ENCODER_OUT);

    // Set GPIO pins as outputs for R encoder
    gpio_set_dir(R_ENCODER_POW, GPIO_OUT);
    gpio_set_dir(R_ENCODER_OUT, GPIO_IN);

    // Set GPIO settings for R encoder
    gpio_pull_up(R_ENCODER_OUT);
    // gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &encoderPulse);

    // Enable the POW pins
    gpio_put(L_ENCODER_POW, 1);
    gpio_put(R_ENCODER_POW, 1);
}

/*
int main() {
    stdio_init_all();

    // Initialise motor GPIO pins and PWM
    initMotorSetup();
    initMotorPWM();

    // Initialise encoder GPIO pins
    initEncoderSetup();

    // Set up a timer to generate interrupts every second
    struct repeating_timer timer;
    add_repeating_timer_ms(1000, encoderCallback, NULL, &timer);

    while (1) {
        // Run at half duty cycle
        moveMotor(1563);
        sleep_ms(5000);

        // Turn left at full duty cycle
        moveMotor(3165);
        turnMotor(1);
        sleep_ms(250);

        // Turn right at full duty cycle
        moveMotor(3165);
        turnMotor(0);
        sleep_ms(250);

        // Run at 32% duty cycle
        // moveMotor(1000);
        moveMotor(3165);
        sleep_ms(5000);
    }

    return 0;
}
*/