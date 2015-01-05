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

#endif
