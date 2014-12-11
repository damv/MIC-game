// GY-80
// Pin configuration
// SCL: P1.1
// SDA: P1.0

unsigned char GYRO_ADR = 0x69;
unsigned char ACCE_ADR = 0x53;
unsigned char MAGN_ADR = 0x1E;

unsigned char I2C_SCL_PIN =;
unsigned char I2C_SDA_PIN =;

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
    STA = 1;    // START flag
    SI = 0;     // SMBUS0 interrupt flag
    while (SI == 0);
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
