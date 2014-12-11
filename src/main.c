#include <c8051f310.h>   
#include <stdio.h>
#define SYSCLK       24500000 / 8         // SYSCLK frequency in Hz

typedef unsigned char event;
event top_second;

sfr16 TMR2RL   = 0xca;                    // Timer2 reload value
sfr16 TMR2     = 0xcc;                    // Timer2 counter

sfr16 ADC0 = 0xbd;
sfr16 PCA0CP0 = 0xFB;

sbit LED = P3^3;                          // LED='1' means ON
sbit SW2 = P0^7;                          // SW2='0' means switch pressed

static int second = 0;

int song[16] = { 
    0x39, 0x39, 0x39, 0x29, 0x3C, 0x3C, 0x29, 0x29, 
    0x39, 0x3C, 0x29, 0x29, 0x39, 0x39, 0x39, 0x39
};
int index = 0;


unsigned int AnBiPot = 0;
unsigned int AnVoPot = 0;
float AnPhiPot = 0;

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void UART0_init(void);
void SYSCLK_Init(void);
void PORT_Init(void);
void Timer2_Init(int counts);
void ADC_Init();
void SMBUS_init();
void SERVO_init();

void SMBUS_begin();
void Put_char_(unsigned char c);
void event_init(event *e);
int event_check(event *e);
void SMBUS_write(unsigned char c);
void SERVO_write(int angle);

//-----------------------------------------------------------------------------
// Function declarations
//-----------------------------------------------------------------------------
void main()
{
    PCA0MD &= ~0x40;                        // disable watchdog timer
    SYSCLK_Init();
    PORT_Init();
    Timer2_Init (SYSCLK / 12 / 500); 
    UART0_init();
    ADC_Init();
    SMBUS_init();
    SERVO_init();

    EA = 1;                                 // enable global interrupts
    
    SMBUS_begin();
    while(1)
    {
        if (event_check(&top_second))
        {
            if (RI0) {
                RI0 = 0;
                switch (SBUF0) {
                    case 'x':
                        PCA0CPH0--;
                        printf("New speed %d\n", (unsigned)(unsigned char)PCA0CPH0 - 244);
                        break;
                    case 'c':
                        PCA0CPH0++;
                        printf("New speed %d\n", (unsigned)(unsigned char)PCA0CPH0 - 244);
                        break;
                    default:
                        break;
                }
            }

            if (PCA0CPH0 > 254) {
                PCA0CPH0 = 254;
            }
            if (PCA0CPH0 < 232) {
                PCA0CPH0 = 232;
            }

            
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

void PORT_Init()
{
    XBR0 = 0x05;
    XBR1 = 0x41;
    P0SKIP = 0xCC;
    P0MDOUT |= 0x10;                        // Tx en push / pull
    P3MDOUT |= 0x08;
    P1MDOUT |= 0x01;
}

void SYSCLK_Init()
{
    OSCICN = 0xC3;                          // change clock frequency
    RSTSRC = 0x04;                          // enable missing clock detector
}

void SMBUS_init()
{
    SMB0CF = 0xC1;
}

void Put_char_(unsigned char c)
{
    while(TI0 == 0);
    TI0 = 0;
    SBUF0 = c;
}

void Timer2_Init (int counts)
{
   TMR2CN  = 0x00;                          // Stop Timer2; Clear TF2;
                                            // use SYSCLK/12 as timebase
   CKCON  &= ~0x60;                         // Timer2 clocked based on T2XCLK;

   TMR2RL  = -counts;                       // Init reload values
   TMR2    = 0xffff;                        // set to reload immediately
   ET2     = 1;                             // enable Timer2 interrupts
   TR2     = 1;                             // start Timer2
}

void ADC_init()
{
    AMX0N = 0x1F; // GND
    AMX0P = 0x00; // P1.0
    ADC0CN |= 0x82;
    ADC0CF = 0x60;
    REF0CN |= 0x0e;
    REF0CN = 0x08;
}

void SERVO_init()
{
    CR = 1;
    PCA0CPM0 = 0xC2;
    PCA0CP0 = -3062; // 1.5ms
    PCA0CPH0 = 248;
}

void SMBUS_begin()
{
    SMBUS_write(0x00);
}

void SMBUS_write(unsigned char c)
{
    // start sequence
    STA = 1;
    SI = 0;
    while (SI == 0);

    // send address 
    STA = 0;
    SMB0DAT = 0x48;
    SI = 0;
    while (SI == 0);

    // send char
    SMB0DAT = c;
    SI = 0;
    while (SI == 0);
}


///////////////////////////
// INTERRUPTS
///////////////////////////

void ADC_ISR() interrupt 10
{
    AD0INT = 0;
    AnBiPot = ADC0;
}

void Timer2_ISR (void) interrupt 5
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
