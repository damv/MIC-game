#include "../lib/c8051f310.h"
#include "utils.h"

#define SYSCLK 24500000        // SYSCLK frequency in Hz

void T0_Wait_ms (unsigned char ms);


void delay(unsigned char ms)
{
    T0_Wait_ms(ms);
}

int positive_modulo(int x, int y)
{
    if (x >= 0) {
        return x % y;
    } else {
        return y + (x % y);
    }
}

//-----------------------------------------------------------------------------
// T0_Wait_ms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) unsigned char ms - number of milliseconds to wait
//                        range is full range of character: 0 to 255
//
// Configure Timer0 to wait for <ms> milliseconds using SYSCLK as its time
// base.
//
void T0_Wait_ms (unsigned char ms)
{
    TCON &= ~0x30;                      // Stop Timer0; Clear TF0
    TMOD &= ~0x0f;                      // 16-bit free run mode
    TMOD |=  0x01;

    CKCON |= 0x04;                      // Timer0 counts SYSCLKs

    while (ms) {
        TR0 = 0;                         // Stop Timer0
        TH0 = -(SYSCLK/1000 >> 8);       // Overflow in 1ms
        TL0 = -(SYSCLK/1000);
        TF0 = 0;                         // Clear overflow indicator
        TR0 = 1;                         // Start Timer0
        while (!TF0);                    // Wait for overflow
        ms--;                            // Update ms counter
    }

    TR0 = 0;                            // Stop Timer0
}
