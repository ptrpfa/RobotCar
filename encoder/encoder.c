// Control L and R encoder

#include "encoder.h"

// Global variable declaration
volatile bool completeMovement = false;
uint32_t targetGridNumber = 0;
uint32_t pulseCountL = 0;
uint32_t pulseCountR = 0;
volatile uint32_t oscillation = 0;
double movedDistance = 0.0;
volatile float actual_speed_L;
volatile float actual_speed_R;

// Function to get motor speed and distance
void getSpeedAndDistance(int encoder, uint32_t pulseCount)
{
    // Calculate motor speed in cm/s
    double distancePerHole = ENCODER_CIRCUMFERENCE / ENCODER_NOTCH;
    double distance = distancePerHole * pulseCount;
    double speed = distance / 0.1;

    // Calculate and accumulate the distance
    movedDistance += distance;

    // Print motor speed and total distance
    if (encoder == 0)
    {
        actual_speed_L = speed;
        // printf("LEFT SPEED: %lf\n\n", actual_speed_L);
    }
    else if (encoder == 1)
    {
        actual_speed_R = speed;
    }
    return;
}

void startTracking(int targetGrids)
{
    movedDistance = 0;              // Reset the distance moved
    targetGridNumber = targetGrids; // set the target number of grids
    completeMovement = false;
    return;
}

uint32_t getGridsMoved()
{
    encoderCallback(); // Calculate final movedDistancce

    uint32_t grids_moved = movedDistance / 14;
    printf("DISTANCE TRAVELLED: %.2lf\n", movedDistance);

    // Reset the distance moved
    movedDistance = 0.0;

    return grids_moved;
}

// Function to count each encoder's pulse
void encoderPulse(uint gpio)
{
    // L encoder interrupted
    if (gpio == L_ENCODER_OUT)
    {
        pulseCountL++;
    }
    // R encoder interrupted
    else if (gpio == R_ENCODER_OUT)
    {
        pulseCountR++;
    }

    oscillation++;
}

// Function to interrupt every second
bool encoderCallback()
{
    // Call getSpeedAndDistance function every second
    getSpeedAndDistance(0, pulseCountL);
    getSpeedAndDistance(1, pulseCountR);

    // Reset the pulse counts
    pulseCountL = 0;
    pulseCountR = 0;

    if (targetGridNumber > 0)
    {
        uint32_t grids_moved = movedDistance / 15.5;
        if (grids_moved >= targetGridNumber)
        {
            targetGridNumber = 0;    // Reset target number of grids
            movedDistance = 0;       // Reset moved distance
            completeMovement = true; // Set flag to indicate target number of grids reached
        }
    }
    return true;
}

// Function to initialize pins for encoders
void initEncoderSetup()
{
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
