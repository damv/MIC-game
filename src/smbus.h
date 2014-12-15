#ifndef __SMBUS_H__
#define __SMBUS_H__

// INIT
// Set config
void SMBUS_init();

// BEGIN
// Initiate connection
void SMBUS_begin();

// WRITE
// Send via bus the specified value to the device corresponding to the specified address
void SMBUS_write(unsigned char address, unsigned char value);

// READ
void SMBUS_read(unsigned char address, unsigned char *readByte, bit isLastRead);

#endif
