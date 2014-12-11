#include <c8051f310.h>  
#include <smbus.h> 
#include <stdio.h>

#define SYSCLK 24500000 / 8         // SYSCLK frequency in Hz

typedef unsigned char event;
event top_second;

sfr16 TMR2RL = 0xca; // Timer2 reload value
sfr16 TMR2 = 0xcc; // Timer2 counter

sfr16 ADC0 = 0xbd;
sfr16 PCA0CP0 = 0xFB;

sbit LED = P3^3; // LED='1' means ON
sbit SW2 = P0^7; // SW2='0' means switch pressed

static int second = 0;

//-----------------------------------------------------------------------------
// Function PROTOTYPES
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
    PCA0MD &= ~0x40; // disable watchdog timer
    SYSCLK_init();
    PORT_init();
    TIMER2_init(SYSCLK / 12 / 500); 
    UART0_init();
    SMBUS_init();

    EA = 1; // enable global interrupts
    
    SMBUS_begin();
    while(1)
    {
        if (event_check(&top_second))
        {

        }
    }
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

void UART0_init()
{
    TH1 = -213;
    TMOD |= 0x20;
    CKCON |= 0x08;
    TR1 = 1;
    REN0 = 1;
    SBUF0 = '\n';
}

void PORT_init()
{
    XBR0 = 0x05;
    XBR1 = 0x41;
    P0SKIP = 0xCC;
    P0MDOUT |= 0x10; // TX in push / pull
    P3MDOUT |= 0x08;
    P1MDOUT |= 0x01;
}

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

///////////////////////////
// INTERRUPTS
///////////////////////////

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
        TF2H = 0;                               // clear Timer2 interrupt flag
        LED = ~LED;                             // change state of LED
    }
}
