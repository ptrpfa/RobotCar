#include <stdint.h>

#define INTERFACE_A 0x1E // Magnetometer slave address
#define MAG_REG 0x03 // data starts on 0x03 - MSb set to 1 (autoincrease on read) -> 0xA8
#define MAG_CRA 0x00
#define MAG_CRB 0x01
#define MAG_MR 0x02

typedef struct {
   int16_t x;
   int16_t y;
   int16_t z;
} accel_t;

typedef struct { // TODO
   int16_t x;
   int16_t y;
   int16_t z;
} mag_t;

void init_i2c_default();
void magnetometer_init();
void read_magnetometer(mag_t *mag);
int32_t get_angle(mag_t *mag);