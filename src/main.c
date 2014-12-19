#include <stdio.h>
#include "../lib/c8051f310.h"  

#include "smbus.h"
#include "spi.h"
#include "utils.h"


#define GYRO_ADDR    0x69   // I2C Addresses
#define ACCE_ADDR    0x53
#define MAGN_ADDR    0x1E

typedef unsigned char event;
event top_second;

sfr16 TMR2RL = 0xca; // Timer2 reload value
sfr16 TMR2 = 0xcc; // Timer2 counter

sfr16 PCA0CP0 = 0xFB;

sbit LED = P3^3; // LED='1' means ON
sbit SW2 = P0^7; // SW2='0' means switch pressed

static int second = 0;

//-----------------------------------------------------------------------------
// Function prototypes
//-----------------------------------------------------------------------------
void UART0_init(void);
void SYSCLK_init(void);
void PORT_init(void);
void TIMER2_init(int counts);

void Put_char_(unsigned char c);
void event_init(event *e);
int event_check(event *e);


//-----------------------------------------------------------------------------
// Function declarations
//-----------------------------------------------------------------------------
void main()
{
    unsigned char readByte0 = 0x00;
    unsigned char readByte1 = 0x00;
    bit isLastBitRead = 1;

    PCA0MD &= ~0x40; // disable watchdog timer
    SYSCLK_init();
    PORT_init();
    TIMER2_init(SYSCLK / 12 / 500); 
    UART0_init();
    SMBUS_init();
    SMBUS_begin(ACCE_ADDR);
    SPI_init();
    screen_init();

    EA = 1; // enable global interrupts
    
    printf("SMBus initialization");

    while(1)
    {
        // SMBUS TEST
        SMBUS_write(ACCE_ADDR, 0x32); // DATAX 0
        SMBUS_read(ACCE_ADDR, &readByte0, isLastBitRead);

        printf(
            "Accel X: %x %x\n",
            (unsigned)(unsigned char)readByte0,
            (unsigned)(unsigned char)readByte1
        );

        T0_Wait_ms(10);

        if (event_check(&top_second))
        {
            // block called each second
        }
    }
}

void UART0_init()
{
    TH1 = -213;
    TMOD |= 0x20;
    CKCON |= 0x08;
    TR1 = 1;
    REN0 = 1;
    SBUF0 = '\n';
}

// Peripheral specific initialization function
void PORT_init()
{
    // P0.0  -  Skipped,     Open-Drain, Digital
    // P0.1  -  Skipped,     Open-Drain, Digital
    // P0.2  -  Skipped,     Open-Drain, Digital
    // P0.3  -  Skipped,     Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Open-Drain, Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Skipped,     Open-Drain, Digital
    // P0.7  -  Skipped,     Open-Drain, Digital

    // P1.0  -  SDA (SMBus), Open-Drain, Digital
    // P1.1  -  SCL (SMBus), Open-Drain, Digital
    // P1.2  -  SYSCLK,      Open-Drain, Digital
    // P1.3  -  T0 (Timr0),  Open-Drain, Digital
    // P1.4  -  T1 (Timr1),  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital
    // P2.0  -  MOSI,        Push-Pull, Digital
    // P2.1  -  MISO,        Push-Pull, Digital
    // P2.2  -  SCK,         Push-Pull, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital

    P2MDOUT   = 0x07;
    P0SKIP    = 0xCF;
    XBR0      = 0x0D;
    XBR1      = 0x70;
    //XBR0      = 0x04;       // Enable SMBus pins?????????????????????????????????
    //XBR1      = 0x40;       // Enable crossbar and weak pull-ups?????????????????
    //P0MDOUT = 0x00;         // All P0 pins open-drain output?????????????????????
    //P0 = 0xFF;
}

//-----------------------------------------------------------------------------
// Timer 2 and uART logs
//-----------------------------------------------------------------------------

void SYSCLK_init()
{
    OSCICN = 0xC3; // change clock frequency
    RSTSRC = 0x04; // enable missing clock detector
}

void Put_char_(unsigned char c)
{
    while(TI0 == 0);
    TI0 = 0;
    SBUF0 = c;
}

void TIMER2_init(int counts)
{
   TMR2CN  = 0x00; // Stop Timer2; Clear TF2;
   CKCON  &= ~0x60; // Timer2 clocked based on T2XCLK;

   TMR2RL  = -counts; // Init reload values
   TMR2    = 0xffff; // set to reload immediately
   ET2     = 1; // enable Timer2 interrupts
   TR2     = 1; // start Timer2
}

void event_init(event *e)
{
    *e = 0;
}

void event_trigger(event *e)
{
    *e = 1;
}

int event_check(event *e)
{
    if (*e == 1) {
        *e = 0;
        return 1;
    }
    else {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Interrupts
//-----------------------------------------------------------------------------
void TIMER2_ISR (void) interrupt 5
{
    static unsigned char x = 5;
    x--;
    if (x == 5)
    {
        x = 5;
        second++;
        event_trigger(&top_second);
    }

    if (TF2H == 1)
    {
        TF2H = 0; // clear Timer2 interrupt flag
        LED = ~LED; // change state of LED
    }
}
