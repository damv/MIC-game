void SMBUS_init()
{
    SMB0CF = 0xC1;
}

void SMBUS_begin(unsigned char address)
{
    SMBUS_write(address, 0x00);
}

void SMBUS_write(unsigned char address, unsigned char value)
{
    // start sequence
    STA = 1;
    SI = 0;
    while (SI == 0);

    // send address 
    STA = 0;
    SMB0DAT = address;
    SI = 0;
    while (SI == 0);

    // send char
    SMB0DAT = value;
    SI = 0;
    while (SI == 0);
}
