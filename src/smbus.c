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

#define POWER_CTL   0x2D


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

    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START aknowledge (?)
    STA = 0;

    // send address R/W = 0
    SMB0DAT = POWER_CTL | READ;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for aknowledge

    // send address R/W = 0
    SMB0DAT = 0x08 | READ;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for aknowledge

}

void SMBUS_write(unsigned char address, unsigned char value)
{
    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START acknowledge (?)
    STA = 0;

    // send address R/W = 0
    SMB0DAT = address | READ;  // SMBUS_DATA > ADRESS, R/W = 0
    SI = 0;
    while (SI == 0);            // Wait for acknowledge

    // send char
    SMB0DAT = value;            // SMBUS DATA
    SI = 0;
    while (SI == 0);            // Wait for acknowledge
}

void SMBUS_read(unsigned char address, unsigned char *readByte, bit isLastRead)
{
    // restart sequence ???
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START acknowledge (?)
    STA = 0;

    // send address R/W = 1
    SMB0DAT = address | WRITE;    // SMBUS_DATA > ADRESS, R/W = 1

    SI = 0;
    while (SI == 0);                // Wait for acknowledge

    // read char
    *readByte = SMB0DAT;
    ACK = !isLastRead;
}
