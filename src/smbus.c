// GY-80
// Pin configuration
// SDA: P1.0
// SCL: P1.1

#include "smbus.h"
#include "utils.h"
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

sbit SCL = P1^1;

void SMBUS_init()
{
    SMB0CF = 0x5D;                      // Use Timer1 overflows as SMBus clock
                                       // source;
                                       // Disable slave mode;
                                       // Enable setup & hold time
                                       // extensions;
                                       // Enable SMBus Free timeout detect;
                                       // Enable SCL low timeout detect;

    SMB0CF |= 0x80;                     // Enable SMBus;
}

void SMBUS_start()
{
    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START send
    STA = 0;            // START flag OFF

    delay(1);
}

void SMBUS_stop()
{
    //test2
    // stop sequence
    STO = 1;            // STOP flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // STOP send
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

unsigned char SMBUS_read_value()
{
    SI = 0;
    while (SI == 0);
    return SMB0DAT;
}

void SMBUS_ack()
{
    ACK = 1;
}

void SMBUS_nack()
{
    ACK = 0;
}

void ACCE_begin()
{   
	SMBUS_start();

    // Power ON
    SMBUS_write_address(ACCE_ADDR, WRITE);
    SMBUS_write_value(POWER_CTRL);
    SMBUS_write_value(0x08); // Measurement Mode
    //SMBUS_stop(); //?????????????????????????????????????????
}

void GYRO_read(
    unsigned char* gyro_x0,
    unsigned char* gyro_x1,
    unsigned char* gyro_y0,
    unsigned char* gyro_y1,
    unsigned char* gyro_z0,
    unsigned char* gyro_z1
)
{
    // send address and write register
    SMBUS_start();
    SMBUS_write_address(GYRO_ADDR, WRITE);
    SMBUS_write_value(0x28);

    // send address in read mode
    SMBUS_start();
    SMBUS_write_address(GYRO_ADDR, READ);
    
    // read char
    *gyro_x0 = SMBUS_read_value();
    SMBUS_ack();
    *gyro_x1 = SMBUS_read_value();
    SMBUS_ack();
    *gyro_y0 = SMBUS_read_value();
    SMBUS_ack();
    *gyro_y1 = SMBUS_read_value();
    SMBUS_ack();
    *gyro_z0 = SMBUS_read_value();
    SMBUS_ack();
    *gyro_z1 = SMBUS_read_value();
    SMBUS_nack();
}



void ACCE_read(
    int* ACCE_X,
    int* ACCE_Y,
    int* ACCE_Z
)
{
	unsigned char ACCE_X0;
	unsigned char ACCE_X1;
	unsigned char ACCE_Y0;
	unsigned char ACCE_Y1;
	unsigned char ACCE_Z0;
	unsigned char ACCE_Z1;

    // send address and write register
    SMBUS_start();
    SMBUS_write_address(ACCE_ADDR, WRITE);
    SMBUS_write_value(DATAX0);

    // send address in read mode
    SMBUS_start();
    SMBUS_write_address(ACCE_ADDR, READ);
    
    // read char
    ACCE_X0 = SMBUS_read_value();
    SMBUS_ack();
    ACCE_X1 = SMBUS_read_value();
    SMBUS_ack();
    ACCE_Y0 = SMBUS_read_value();
    SMBUS_ack();
    ACCE_Y1 = SMBUS_read_value();
    SMBUS_ack();
    ACCE_Z0 = SMBUS_read_value();
    SMBUS_ack();
    ACCE_Z1 = SMBUS_read_value();
    SMBUS_nack();

    *ACCE_X = ((int)ACCE_X1 << 8) | ACCE_X0;
    *ACCE_Y = ((int)ACCE_Y1 << 8) | ACCE_Y0;
    *ACCE_Z = ((int)ACCE_Z1 << 8) | ACCE_Z0;

    // SMBUS_stop();
}

