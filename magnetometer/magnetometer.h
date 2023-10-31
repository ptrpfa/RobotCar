#ifndef __LSM303DLH__
#define __LSM303DLH__

#include <stdint.h>


#define INTERFACE_A 0x18 // Accelerometr slave address
#define INTERFACE_B 0x1E // Magnetometr slave address


#define CTRL_REG_1 0x20
#define CTRL_REG_4 0x23
#define ACC_REG 0xA8 // data starts on 0x28 - MSb set to 1 (autoincrease on read) -> 0xA8
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

void lsm303dlh_acc_setup();

void lsm303dlh_mag_setup();

void lsm303dlh_read_acc(accel_t *acc);

void lsm303dlh_read_mag(mag_t *mag);

int32_t get_angle(mag_t *mag);

#endif
