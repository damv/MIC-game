#ifndef __SMBUS_H__
#define __SMBUS_H__

#define  SMB_FREQUENCY  10000          // Target SCL clock rate

// INIT
void SMBUS_init();

// BEGIN
void ACCE_begin();

// READ
void ACCE_read(
    unsigned char* ACCE_X0,
    unsigned char* ACCE_X1,
    unsigned char* ACCE_Y0,
    unsigned char* ACCE_Y1,
    unsigned char* ACCE_Z0,
    unsigned char* ACCE_Z1
);

// void GYRO_read(
//     unsigned char* gyro_x0,
//     unsigned char* gyro_x1,
//     unsigned char* gyro_y0,
//     unsigned char* gyro_y1,
//     unsigned char* gyro_z0,
//     unsigned char* gyro_z1
// );

#endif
