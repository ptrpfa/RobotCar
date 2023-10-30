// Control L and R motor speed

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "motor.h"

// Function to initialize pins for motors
void initMotorSetup() {
    // Initialize GPIO pins for L motor control
    gpio_init(L_MOTOR_IN1);
    gpio_init(L_MOTOR_IN2);
    gpio_init(L_MOTOR_ENA);

    // Initialize GPIO pins for R motor control
    gpio_init(R_MOTOR_IN3);
    gpio_init(R_MOTOR_IN4);
    gpio_init(R_MOTOR_ENB);

    // Set GPIO pins as outputs for L motor
    gpio_set_dir(L_MOTOR_IN1, GPIO_OUT);
    gpio_set_dir(L_MOTOR_IN2, GPIO_OUT);
    gpio_set_dir(L_MOTOR_ENA, GPIO_OUT);

    // Set GPIO pins as outputs for R motor
    gpio_set_dir(R_MOTOR_IN3, GPIO_OUT);
    gpio_set_dir(R_MOTOR_IN4, GPIO_OUT);
    gpio_set_dir(R_MOTOR_ENB, GPIO_OUT);    

    // Enable the EN pins
    gpio_put(L_MOTOR_ENA, 1);
    gpio_put(R_MOTOR_ENB, 1);
}

// Function to initialize PWMs for motors
void initMotorPWM() {
    // Set GPIO pins for ENA and ENB to PWM mode
	gpio_set_function(L_MOTOR_ENA, GPIO_FUNC_PWM);
	gpio_set_function(R_MOTOR_ENB, GPIO_FUNC_PWM);

    // Get PWM slice and channel for ENA and ENB
	uint sliceLeft = pwm_gpio_to_slice_num(L_MOTOR_ENA);
	uint channelLeft = pwm_gpio_to_channel(L_MOTOR_ENA);
	uint sliceRight = pwm_gpio_to_slice_num(R_MOTOR_ENB);
	uint channelRight = pwm_gpio_to_channel(R_MOTOR_ENB);

    // Set PWM frequency to 40kHz (125MHz / 3125)
	pwm_set_wrap(sliceLeft, 3125);              
	pwm_set_wrap(sliceRight, 3125);

    // Set clock divider to 125
	pwm_set_clkdiv(sliceLeft, 125);
	pwm_set_clkdiv(sliceRight, 125);

    // Enable PWM for both motor channels
	pwm_set_enabled(sliceLeft, true);	        
	pwm_set_enabled(sliceRight, true);          
}

// Function to move forward
void moveMotor(float pwm) {
    // Set both motors to output high for desired PWM
    pwm_set_chan_level(pwm_gpio_to_slice_num(L_MOTOR_ENA), pwm_gpio_to_channel(L_MOTOR_ENA), pwm);
    pwm_set_chan_level(pwm_gpio_to_slice_num(R_MOTOR_ENB), pwm_gpio_to_channel(R_MOTOR_ENB), pwm);

    // Turn on both motors
    gpio_put(L_MOTOR_IN1, 0);
    gpio_put(L_MOTOR_IN2, 1);
    gpio_put(R_MOTOR_IN3, 0);
    gpio_put(R_MOTOR_IN4, 1);

    // Enable the enable pins
    gpio_put(L_MOTOR_ENA, 1);
    gpio_put(R_MOTOR_ENB, 1);
}

// Function to stop
void stopMotor() {
    // Turn off all motors
    gpio_put(L_MOTOR_IN1, 0);
    gpio_put(L_MOTOR_IN2, 0);
    gpio_put(R_MOTOR_IN3, 0);
    gpio_put(R_MOTOR_IN4, 0);

    // Disable the enable pins
    gpio_put(L_MOTOR_ENA, 0);
    gpio_put(R_MOTOR_ENB, 0);
}

// Function to turn
// 0 - left, 1 - right 
void turnMotor(int direction) {
    // Motor to turn left 
    if (direction == 0) {
        // Reverse left wheel, forward right wheel
        gpio_put(L_MOTOR_IN1, 1);
        gpio_put(L_MOTOR_IN2, 0);
        gpio_put(R_MOTOR_IN3, 0);
        gpio_put(R_MOTOR_IN4, 1);

        // Enable the enable pins
        gpio_put(L_MOTOR_ENA, 1);
        gpio_put(R_MOTOR_ENB, 1);

        sleep_ms(380);
    }   
    // Motor to turn right
    else {
        // Reverse right wheel, forward left wheel
        gpio_put(L_MOTOR_IN1, 0);
        gpio_put(L_MOTOR_IN2, 1);
        gpio_put(R_MOTOR_IN3, 1);
        gpio_put(R_MOTOR_IN4, 0);

        // Enable the enable pins
        gpio_put(L_MOTOR_ENA, 1);
        gpio_put(R_MOTOR_ENB, 1);

        sleep_ms(380);
    }

    // Stop motor after turning
    stopMotor();
}

/*
int main() {
    stdio_init_all();

    // Initialise motor GPIO pins
    initMotorSetup();

    // Initialise motor PWM
    initMotorPWM();

    while (1) {
        // Run at half duty cycle for 2 seconds
        moveMotor(1563);
        sleep_ms(2000);

        // Turn right for 1 second
        turnMotor(1)
        sleep_ms(1000);

        // Run at full duty cycle for 2 seconds
        moveMotor(3125);
        sleep_ms(2000);

        // Turn left for 1 second
        turnMotor(0)
        sleep_ms(1000);

        // Stop for 5 seconds
        stopMotor()
        sleep_ms(5000);
    }

    return 0;
}
*/