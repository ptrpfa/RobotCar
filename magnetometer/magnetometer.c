#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "magnetometer.h"

#define MAGNETOMETER_ADDRESS 0x1E       // The I2C address of the magnetometer
#define I2C_BAUD 400                    // 400 or 100 (kHz)
#define REFRESH_PERIOD 1080             // ms

void init_i2c_default() {
   i2c_init(i2c_default, I2C_BAUD * 1000);
   gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
   gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
   gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

void magnetometer_init() {
   uint8_t buffer[2] = {MAG_CRA, 0x10}; // 15 Hz refreshrate
   
   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      2,
      true
   );

   buffer[0] = MAG_CRB;
   buffer[1] = 0xE0;                    // Gain - range +-8.1 Gauss, Gain X/Y and Z [LSB/Gauss] 230, GainZ [LSB/Gauss] 205
   
   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      2,
      true
   );

   buffer[0] = MAG_MR;
   buffer[1] = 0x00;                    // Continuous-conversion mode
   
   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      2,
      false
   );

}

// read mag value x, y, z
void read_magnetometer(mag_t *mag) {
   uint8_t buffer[6];
   int16_t magnet[3];
   uint8_t reg = MAG_REG;

   i2c_write_blocking(
      i2c_default,
      INTERFACE_A,
      &reg,
      1,
      true
   );

   i2c_read_blocking(
      i2c_default,
      INTERFACE_A,
      buffer,
      6,
      false
   );

   // merge uint8_t to int16_t
   for (int i = 0; i < 3; i++) {
      magnet[i] = ((buffer[i * 2] << 8 | buffer[(i * 2) + 1] ));
   }

   mag->x = magnet[0];
   mag->y = magnet[1];
   mag->z = magnet[2];
}

int32_t get_angle(mag_t *mag) {
   int32_t angle = 0; 
   // Get angle in degrees
   angle = atan2(mag->x, mag->y)*180.0/M_PI;
   if(angle < 0){
    angle += 360;
   }
   return angle;
}

int main() {
   
   // init all
   init_i2c_default();
   stdio_init_all();
   magnetometer_init();

   mag_t mag;

   // read values
   while (true) {
      read_magnetometer(&mag);
      int32_t angle = get_angle(&mag); 
      printf("\nMag. \nX = %4d, \nY = %4d, \nZ = %4d \n",
            mag.x,mag.y,mag.z);
      printf("Angle: %d degrees", angle);
      sleep_ms(REFRESH_PERIOD);
   }

   return 0;
}