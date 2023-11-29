// Main program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "magnetometer.h"
#include "ultrasonic.h"
#include "wallsensor.h"
#include "barcode.h"
#include "encoder.h"
#include "motor.h"
#include "ssi.h"
#include "cgi.h"
#include "navigation.h"
#include "mapping.h"

const char WIFI_SSID[] = "P";            // Wifi credentials
const char WIFI_PASSWORD[] = "embedded"; // Wifi credentials

// Function that is invoked upon a change in right IR sensor's input
void callbacks(uint gpio, uint32_t events)
{
    switch (gpio)
    {
    // Left wheel encoder callback
    case L_ENCODER_OUT:
        encoderPulse(L_ENCODER_OUT);
        break;
    // Right wheel encoder callback
    case R_ENCODER_OUT:
        encoderPulse(R_ENCODER_OUT);
        break;
    // Ultrasonic callback
    case ECHOPIN:
        get_echo_pulse(ECHOPIN, events);
        break;
    // Barcode sensor callback
    case IR_SENSOR_PIN:
        barcode_callback(gpio, events);
        break;
    default:
        break;
    }
}

// Function to init all sensors and motors
void initAll()
{
    // Initialise standard I/O
    stdio_init_all();
    sleep_ms(1000);

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    sleep_ms(1000);

    // Loop until connected to WiFI network
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0)
    {
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected!\n");

    // Initialise web server
    httpd_init();
    printf("1/9 - Http server initialised\n");
    sleep_ms(500);

    // Initialise SSI and CGI handler
    ssi_init();
    cgi_init();
    printf("2/9 - SSI and CGI Handler initialised\n");
    sleep_ms(500);

    // Initialise motor pins and PWM
    initMotorSetup();
    initMotorPWM();
    printf("3/9 - Motor pins and PWM initialised\n");
    sleep_ms(500);

    // Initialise encoder pins and setup timer to generate interrupts every second to update speed and distance
    initEncoderSetup();
    printf("4/9 - Wheel encoder pins initialised\n");
    sleep_ms(500);

    // Initialise ultrasonic sensor
    setupUltrasonicPins();
    printf("5/9 - Ultrasonic pins initialised\n");
    sleep_ms(500);

    // Initialise wall sensors
    init_wallsensors();
    printf("6/9 - Wall sensor pins initialised\n");
    sleep_ms(500);

    // Initialise barcode sensor pin
    init_barcode();
    printf("7/9 - Barcode sensor pin initialised\n");
    sleep_ms(500);

    init_i2c_default();
    magnetometer_init();
    printf("8/9 - Magnetometer pins initialised\n");
    sleep_ms(500);

    initializeMazeGrid();
    printf("9/9 - Maze grids initialised\n");
}

// Function to init all interrupts
void initInterrupts()
{
    // Initialise interrupts for needed sensors
    gpio_set_irq_enabled_with_callback(L_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(R_ENCODER_OUT, GPIO_IRQ_EDGE_RISE, true, &callbacks);
    gpio_set_irq_enabled_with_callback(ECHOPIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
    gpio_set_irq_enabled_with_callback(IR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &callbacks);
}

int main()
{
    // Init all required
    initAll();
    initInterrupts();

    // Init PID
    add_repeating_timer_ms(75, pidUpdateCallback, NULL, &pid_timer);

    double cm;

    while (startCar == 0)
    {
        // Wait
    }

    // Maybe change below into a function for cgi

    mapMaze(STARTING_X, STARTING_Y);

    printf("** MAZE MAPPING COMPLETED **\n");
    printf("** MAP DETAILS **\n");

    for (int x = 0; x < MAZE_WIDTH; x++)
    {
        for (int y = 0; y < MAZE_HEIGHT; y++)
        {
            printf("X: %d, Y: %d, SOUTHWALL: %d, WESTWALL: %d, NORTHWALL: %d, EASTWALL: %d, VISITED: %d\n\n", x, y, mazeGrid[x][y].southWall, mazeGrid[x][y].westWall, mazeGrid[x][y].northWall, mazeGrid[x][y].eastWall, mazeGrid[x][y].visited);
        }
    }
    isMazeMapped = true;
    printf("TIME TO NAVIGATE\n");

    //  For reference
    // // Solve maze
    // solveMaze(STARTING_X, STARTING_Y, ENDING_X, ENDING_Y);
    // // cancel_repeating_timer(&pid_timer);
    // // sleep_ms(10000);
    // startCar = 0;

    return 0;
}
