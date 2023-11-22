#include "cgi.h"
#include "barcode.h"

// CGI handler which is run when a request for /led.cgi is detected
const char *cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if an request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0], "led") == 0)
    {
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if (strcmp(pcValue[0], "0") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        else if (strcmp(pcValue[0], "1") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }

    // Send the index page back to the user
    return "/index.shtml";
}

// CGI handler which is run when a request for /engine.cgi is detected
const char *cgi_engine_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if a request for the engine has been made (/engine.cgi?engine=x)
    if (strcmp(pcParam[0], "engine") == 0)
    {
        // Look at the argument to check if the engine is to be started (x=1) or stopped (x=0)
        if (strcmp(pcValue[0], "0") == 0)
        {
            // Stop the motor
            startCar = 0;
        }
        else if (strcmp(pcValue[0], "1") == 0)
        {
            // Start the motor (you can replace 1.0f with the desired PWM value)
            startCar = 1;
        }
    }

    // Send the index page back to the user
    return "/index.shtml";
}

const char *cgi_barcode_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if a request for the engine has been made (/barcode.cgi?barcode=x)
    if (strcmp(pcParam[0], "barcode") == 0)
    {
        // Look at the argument to check if the engine is to be started (x=1) or stopped (x=0)
        if (strcmp(pcValue[0], "1") == 0)
        {
            // Start the motor (you can replace 1.0f with the desired PWM value)
            reset_barcode();
        }
    }

    // Send the index page back to the user
    return "/index.shtml";
}

// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {// Html request for "/led.cgi" triggers cgi_led_handler
     "/led.cgi", cgi_led_handler},
    {// Html request for "/engine.cgi" triggers cgi_engine_handler
     "/engine.cgi", cgi_engine_handler},
    {// Html request for "/engine.cgi" triggers cgi_engine_handler
     "/barcode.cgi", cgi_barcode_handler},
};

void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, LWIP_ARRAYSIZE(cgi_handlers));
}
