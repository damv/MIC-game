// GY-80
// Pin configuration
// SDA: P1.0
// SCL: P1.1
#include "smbus.h"
#include "../lib/c8051f310.h"

#define GYRO_ADDR    0x69   // I2C Addresses
#define ACCE_ADDR    0x53
#define MAGN_ADDR    0x1E

#define WRITE       0x00    // SMBus WRITE command
#define READ        0x01    // SMBus READ command

void SMBUS_init()
{
    // SMBUS CONFIGURATION
    // smbus enabled
    // slave inhibited
    // clock source : timer 1 overflow
    SMB0CF = 0xC1;
}

void SMBUS_begin(unsigned char address)
{
    // Initiate connection
    SMBUS_write(address, 0x00);
}

void SMBUS_write(unsigned char address, unsigned char value)
{
    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START aknowledge (?)
    STA = 0;


    // send address R/W = 0
    SMB0DAT = address | 0x00;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for aknowledge

    // send char
    SMB0DAT = value;            // SMBUS DATA
    SI = 0;
    while (SI == 0);            // Wait for aknowledge

    // stop sequence
    STO = 1;            // STOP flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // STOP aknowledge (?)
    STO = 0;
}

void SMBUS_read(unsigned char address, unsigned char *readByte, bit isLastRead)
{
    // restart sequence ???
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START aknowledge (?)
    STA = 0;

    // send address R/W = 1
    SMB0DAT = address | 0x01;       // SMBUS_DATA > ADRESS, R/W = 1
    SI = 0;
    while (SI == 0);                // Wait for aknowledge

    // stop sequence
    STO = 1;            // STOP flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // STOP aknowledge (?)
    STO = 0;

    // read char
    *readByte = SMB0DAT;
    ACK = !isLastRead;
}
