#include <stdio.h>
#include <stdlib.h>

#include "../lib/c8051f310.h"

#include "smbus.h"
#include "spi.h"
#include "utils.h"
#include "game.h"

#define SYSCLK 24500000         // SYSCLK frequency in Hz

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
void TIMER1_init();
void TIMER2_init(int counts);

void Put_char_(unsigned char c);
void event_init(event *e);
int event_check(event *e);

//-----------------------------------------------------------------------------
// Function declarations
//-----------------------------------------------------------------------------

void main()
{
    unsigned int screenPos = 0;
    unsigned int screenSpeed = 0;
    unsigned char readByte0 = 0x00;
    unsigned char readByte1 = 0x00;
    Game game;
    
    signed int ACCE_X = 0x10;
    signed int ACCE_Y = 0x10;
    signed int ACCE_Z = 0x10;

    PCA0MD &= ~0x40; // disable watchdog timer

    SYSCLK_init();
    PORT_init();
    TIMER1_init();
    TIMER2_init(SYSCLK / 12 / 500); 
    UART0_init();

	printf("\n===========================\n");
	printf("  Welcome\n");
	printf("===========================\n");

    SMBUS_init();
	printf("- SMBUS intialized\n");
    SPI_init();
	printf("- SPI intialized\n");
    screen_init();
    screen_initScroll();
	printf("- Screen initialized\n");
    ACCE_begin();
	printf("- Accelerometer communication started\n");

    EA = 1; // enable global interrupts


    game_init(&game);

    while(1)
    {
        // SMBUS TEST
        ACCE_read(&ACCE_X, &ACCE_Y, &ACCE_Z);

        printf(
            "Accelerometer : %d\t%d\t%d\n",
            (int)ACCE_X,
            (int)ACCE_Y,
            (int)ACCE_Z
        );

        screenSpeed = -ACCE_Y / 10;
        screenPos = positive_modulo((screenPos + screenSpeed - 1), SCREEN_SCROLLING_HEIGHT);

        game.player.x += ACCE_X / 10;
        game.player.y += screenSpeed;

        game_draw(game.player);

        screen_verticalScroll(screenPos);
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
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  SCREEN_CS,   Push-Pull, Digital
    // P1.6  -  SCREEN_RST,  Push-Pull, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital
    // P2.0  -  SPI_MOSI,    Push-Pull, Digital
    // P2.1  -  SPI_MISO,    Open-Drain, Digital
    // P2.2  -  SPI_SCK,     Push-Pull, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital

    P1MDOUT   = 0x60;
    P2MDOUT   = 0x05;
    P0SKIP    = 0xCF;
    XBR0      = 0x05;
    XBR1      = 0x40;
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
// Timer1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Timer1 configured as the SMBus clock source as follows:
// - Timer1 in 8-bit auto-reload mode
// - SYSCLK or SYSCLK / 4 as Timer1 clock source
// - Timer1 overflow rate => 3 * SMB_FREQUENCY
// - The resulting SCL clock rate will be ~1/3 the Timer1 overflow rate
// - Timer1 enabled
//
void TIMER1_init()
{

// Make sure the Timer can produce the appropriate frequency in 8-bit mode
// Supported SMBus Frequencies range from 10kHz to 100kHz.  The CKCON register
// settings may need to change for frequencies outside this range.
#if ((SYSCLK/SMB_FREQUENCY/3) < 255)
   #define SCALE 1
      CKCON |= 0x08;                   // Timer1 clock source = SYSCLK
#elif ((SYSCLK/SMB_FREQUENCY/4/3) < 255)
   #define SCALE 4
      CKCON |= 0x01;
      CKCON &= ~0x0A;                  // Timer1 clock source = SYSCLK / 4
#endif

   TMOD = 0x20;                        // Timer1 in 8-bit auto-reload mode

   // Timer1 configured to overflow at 1/3 the rate defined by SMB_FREQUENCY
   TH1 = -(SYSCLK/SMB_FREQUENCY/SCALE/3);

   TL1 = TH1;                          // Init Timer1

   TR1 = 1;                            // Timer1 enabled
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
