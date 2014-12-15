// GY-80
// Pin configuration
// SDA: P1.0
// SCL: P1.1
#include "smbus.h"
#include "../lib/c8051f310.h"

#define GYRO_ADDR    0x69   // I2C Addresses
#define ACCE_ADDR    0x53
#define MAGN_ADDR    0x1E

#define POWER_CTL   0x2D


void SMBUS_init()
{
    // SMBUS CONFIGURATION
    // smbus enabled
    // slave inhibited
    // clock source : timer 1 overflow
    SMB0CF = 0xC1;
}

void ACCE_begin()
{
    SMBUS_start

    // send address R/W = 0
    SMB0DAT = address | READ;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for acknowledge
    
    // POWER_CTRL register
    SMB0DAT = POWER_CTL | READ;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for aknowledge

    // value : 0x08
    SMB0DAT = 0x08 | READ;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for aknowledge
}

void SMBUS_start()
{
    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START aknowledge
    STA = 0;            // START flag OFF
}


void SMBUS_stop(unsigned char address)
{
    // stop sequence
    STO = 1;            // STOP flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // STOP acknowledge (?)
    STO = 0;
}

void SMBUS_write_address(unsigned char value, unsigned char RW)
{
    // send address
    SMB0DAT = address | RW;
    SI = 0;
    while (SI == 0);            // Wait for acknowledge
}

void SMBUS_write_value(unsigned char value)
{
    // send char
    SMB0DAT = value;            // SMBUS DATA
    SI = 0;
    while (SI == 0);            // Wait for acknowledge
}

void SMBUS_read(unsigned char address, unsigned char *readByte, bit isLastRead)
{
    // // restart sequence ???
    // STA = 1;            // START flag
    // SI = 0;             // SMBUS0 interrupt flag
    // while (SI == 0);    // START acknowledge (?)
    // STA = 0;

    // send address RW = 1
    SMB0DAT = address | 0x01;    // SMBUS_DATA > ADRESS, R/W = 1

    SI = 0;
    while (SI == 0);                // Wait for acknowledge

    // read char
    *readByte = SMB0DAT;
    ACK = !isLastRead;
}
