// GY-80
// Pin configuration
// SDA: P1.0
// SCL: P1.1

#define GYRO_ADR        0x69
#define ACCE_ADR        0x53
#define MAGN_ADR        0x1E

#define  WRITE          0x00           // SMBus WRITE command
#define  READ           0x01           // SMBus READ command


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


    // send address 
    SMB0DAT = address;  // SMBUS DATA
    SI = 0;
    while (SI == 0);

    // send char
    SMB0DAT = value;
    SI = 0;
    while (SI == 0);
}

void SMBUS_read(unsigned char address, unsigned char value)
{
    // start sequence
    STA = 1;            // START flag
    SI = 0;             // SMBUS0 interrupt flag
    while (SI == 0);    // START aknowledge (?)
    STA = 0;


    // send address 
    SMB0DAT = address;  // SMBUS DATA
    SI = 0;
    while (SI == 0);

    // send char
    SMB0DAT = value;
    SI = 0;
    while (SI == 0);
}
