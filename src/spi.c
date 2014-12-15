// HX8340B 2.2" TFT screen
// Pin configuration
// MOSI: P2.0
// MISO: P2.1
// SCK: P2.2

// SPI mode 0
// CPOL 0, CPHA 0

#include "../lib/c8051f310.h"
#include "spi.h"
#include "utils.h"

sbit MOSI = P2^0;
sbit SCK = P2^2;

void delay(int ms)
{
    T0_Wait_ms(ms);
}

void screen_init()
{
    // taken from https://github.com/adafruit/Adafruit-HX8340B/blob/master/Adafruit_HX8340B.cpp
    // 14 commands in list
    
    // 1: ???, 3 args, no delay
    SPI_write(HX8340B_N_SETEXTCMD);
    SPI_write(0xFF);
    SPI_write(0x83);
    SPI_write(0x40);
    
    // 2: No args, delay follows
    SPI_write(HX8340B_N_SPLOUT);
    delay(150);
    
    // 3: Undoc'd register?  3 args, no delay
    SPI_write(0xCA);
    SPI_write(0x70);
    SPI_write(0x00);
    SPI_write(0xD9);
    
    // 4: Undoc'd register?  2 args, no delay
    SPI_write(0xB0);
    SPI_write(0x01);
    SPI_write(0x11);
    
    // 5: Drive ability, 8 args + delay
    SPI_write(0xC9);
    SPI_write(0x90);
    SPI_write(0x49);
    SPI_write(0x10);
    SPI_write(0x28);
    SPI_write(0x28);
    SPI_write(0x10);
    SPI_write(0x00);
    SPI_write(0x06);
    delay(20);
    
    // 6: Positive gamma control, 9 args
    SPI_write(HX8340B_N_SETGAMMAP);
    SPI_write(0x60);
    SPI_write(0x71);
    SPI_write(0x01); // 2.2
    SPI_write(0x0E);
    SPI_write(0x05);
    SPI_write(0x02);
    SPI_write(0x09);
    SPI_write(0x31);
    SPI_write(0x0A);
    
    // 7: Negative gamma, 8 args + delay
    SPI_write(HX8340B_N_SETGAMMAN);
    SPI_write(0x67);
    SPI_write(0x30);
    SPI_write(0x61);
    SPI_write(0x17); // 2.2
    SPI_write(0x48);
    SPI_write(0x07);
    SPI_write(0x05);
    SPI_write(0x33);
    delay(10);
    
    // 8: Power Control 5, 3 args
    SPI_write(HX8340B_N_SETPWCTR5);
    SPI_write(0x35);
    SPI_write(0x20);
    SPI_write(0x45);
    
    // 9: Power control 4, 3 args + delay
    SPI_write(HX8340B_N_SETPWCTR4);
    SPI_write(0x33);
    SPI_write(0x25);
    SPI_write(0x4c);
    delay(10);
    
    // 10: Color Mode, 1 arg
    SPI_write(HX8340B_N_COLMOD);
    SPI_write(0x05); // 0x05 = 16bpp, 0x06 = 18bpp
    
    // 11: Display on, no args, w/delay
    SPI_write(HX8340B_N_DISPON);
    delay(10);

    // 12: Physical column pointer, 4 args
    SPI_write(HX8340B_N_CASET);
    SPI_write(0x00);
    SPI_write(0x00);
    SPI_write(0x00);
    SPI_write(0xaf); // 175 (max X)

    // 13: Physical page pointer, 4 args
    SPI_write(HX8340B_N_PASET);
    SPI_write(0x00);
    SPI_write(0x00);
    SPI_write(0x00);
    SPI_write(0xdb); // 219 (max Y)

    // 14: Start GRAM write
    SPI_write(HX8340B_N_RAMWR);
}


void SPI_write(unsigned short c)
{
    char i = 7;
    
    // send 1 as first bit
    MOSI = 1;
    SCK = 1;
    SCK = 0;

    // send actual data
    for (i = 7; i >= 0; i--) {
        MOSI = (c >> i) & 1; // send data through MOSI
        SCK = 1; // clock tick
        SCK = 0; // clock tock
    }
}
