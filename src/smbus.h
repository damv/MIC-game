#ifndef __SMBUS_H__
#define __SMBUS_H__

#define  SMB_FREQUENCY  10000          // Target SCL clock rate

// INIT
void SMBUS_init();

// BEGIN
void ACCE_begin();

// READ
void ACCE_read(
    int* ACCE_X,
    int* ACCE_Y,
    int* ACCE_Z,
);

#endif
