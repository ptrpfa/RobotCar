// Control L and R motor speed

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "motor.h"

// Initialize pins for motor
void initMotorSetup() {
    // Initialize GPIO pins for L motor control
    gpio_init(IN1);
    gpio_init(IN2);
    gpio_init(ENA);

    // Initialize GPIO pins for R motor control
    gpio_init(IN3);
    gpio_init(IN4);
    gpio_init(ENB);

    // Set GPIO pins as outputs for L motor
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_set_dir(ENA, GPIO_OUT);

    // Set GPIO pins as outputs for R motor
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_set_dir(IN4, GPIO_OUT);
    gpio_set_dir(ENB, GPIO_OUT);    

    // Enable the EN pins
    gpio_put(ENA, 1);
    gpio_put(ENB, 1);
}

// Initialize PWMs for motor
void initMotorPWM() {
    // Set GPIO pins for ENA and ENB to PWM mode
	gpio_set_function(ENA, GPIO_FUNC_PWM);
	gpio_set_function(ENB, GPIO_FUNC_PWM);

    // Get PWM slice and channel for ENA and ENB
	uint sliceLeft = pwm_gpio_to_slice_num(ENA);
	uint channelLeft = pwm_gpio_to_channel(ENA);
	uint sliceRight = pwm_gpio_to_slice_num(ENB);
	uint channelRight = pwm_gpio_to_channel(ENB);

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
    pwm_set_chan_level(pwm_gpio_to_slice_num(ENA), pwm_gpio_to_channel(ENA), pwm);
    pwm_set_chan_level(pwm_gpio_to_slice_num(ENB), pwm_gpio_to_channel(ENB), pwm);

    // Turn on both motors
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);

    // Enable the enable pins
    gpio_put(ENA, 1);
    gpio_put(ENB, 1);
}

// Function to stop
void stopMotor() {
    // Turn off all motors
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);

    // Disable the enable pins
    gpio_put(ENA, 0);
    gpio_put(ENB, 0);
}

// Function to turn
// 0 - left, 1 - right 
void turnMotor(int direction) {
    // Motor to turn left 
    if (direction == 0) {
        // Reverse left wheel, forward right wheel
        gpio_put(IN1, 1);
        gpio_put(IN2, 0);
        gpio_put(IN3, 0);
        gpio_put(IN4, 1);

        // Enable the enable pins
        gpio_put(ENA, 1);
        gpio_put(ENB, 1);

        sleep_ms(250);
    }   
    // Motor to turn right
    else {
        // Reverse right wheel, forward left wheel
        gpio_put(IN1, 0);
        gpio_put(IN2, 1);
        gpio_put(IN3, 1);
        gpio_put(IN4, 0);

        // Enable the enable pins
        gpio_put(ENA, 1);
        gpio_put(ENB, 1);

        sleep_ms(250);
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
        // Run at half of duty cycle
        moveMotor(1563);
        
        sleep_ms(3000);

        // Turn right
        turnMotor(1);

        // Run at full duty cycle
        moveMotor(3125);

        sleep_ms(3000);
    }

    return 0;
}
*/