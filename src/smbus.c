// GY-80
// Pin configuration
// SDA: P1.0
// SCL: P1.1

#include "smbus.h"
#include "../lib/c8051f310.h"

// I2C Addresses (7 bits)
#define GYRO_ADDR   0x69
#define ACCE_ADDR   0x53
#define MAGN_ADDR   0x1E

// ACCE Registers
#define POWER_CTRL  0x2D
#define DATAX0      0x32

#define WRITE       0x00
#define READ        0x01

void SMBUS_init()
{
    // SMBUS CONFIGURATION
    // smbus enabled
    // slave inhibited
    // clock source : timer 1 overflow
    SMB0CF = 0xC1;
}

void SMBUS_start()
{
    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START aknowledge
    STA = 0;            // START flag OFF
}

void SMBUS_stop()
{
    //test2
    // stop sequence
    STO = 1;            // STOP flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // STOP acknowledge
    STO = 0;
}

void SMBUS_write_address(unsigned char address, unsigned char RW)
{
    // send address
    SMB0DAT = (address << 1) | RW;
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


void ACCE_begin()
{
    SMBUS_start();

    // Power ON
    SMBUS_write_address(ACCE_ADDR, WRITE);
    SMBUS_write_value(POWER_CTRL);
    SMBUS_write_value(0x08);    // valeur ?????????????????
    // SMBUS_stop() ???????????????????????????????????????
}

void ACCE_read()
{
    unsigned char ACCE_X0;
    unsigned char ACCE_X1;
    unsigned char ACCE_Y0;
    unsigned char ACCE_Y1;
    unsigned char ACCE_Z0;
    unsigned char ACCE_Z1;

    // SMBUS_start() ???????????????????????????????????????

    SMBUS_write_address(ACCE_ADDR, READ);
    SMBUS_write_value(DATAX0);

    // read char
    ACCE_X0 = SMB0DAT;
    ACK = 1;
    ACCE_X1 = SMB0DAT;
    ACK = 1;
    ACCE_Y0 = SMB0DAT;
    ACK = 1;
    ACCE_Y1 = SMB0DAT;
    ACK = 1;
    ACCE_Z0 = SMB0DAT;
    ACK = 1;
    ACCE_Z1 = SMB0DAT;
    ACK = 1; // OU ACK = 0 ??????????????????????????????????
}

