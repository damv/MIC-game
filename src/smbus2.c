//-----------------------------------------------------------------------------
// F31x_SMBus_Master_Multibyte.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Example software to demonstrate the C8051F31x SMBus interface in
// Master mode.
// - Interrupt-driven SMBus implementation
// - Only master states defined (no slave or arbitration)
// - multiple-byte SMBus data holders used for each transmit and receive
// - Timer1 used as SMBus clock source
// - Timer3 used by SMBus for SCL low timeout detection
// - SCL frequency defined by <SMB_FREQUENCY> constant
// - ARBLOST support included
// - supports multiple-byte writes and multiple-byte reads
// - Pinout:
//    P0.0 -> SDA (SMBus)
//    P0.1 -> SCL (SMBus)
//
//    P3.0 -> C2D (debugging interface)
//
//    P3.3 -> LED (on the 'F310 TB)
//
//    all other port pins unused
//
// How To Test:
//
// 1) Download code to a 'F31x device that is connected to a SMBus slave.
// 2) Run the code:
//         a) The test will indicate proper communication with the slave by
//            toggling the LED on and off each time a value is sent and
//            received.
//         b) The best method to view the proper functionality is to run to
//            the indicated line of code in the TEST CODE section of main and
//            view the SMB_DATA_IN and SMB_DATA_OUT variable arrays in the
//            Watch Window.
//
//
// FID:            31X000006
// Target:         C8051F31x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -30 MAR 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F310.h>
#include "utils.h"
#include "smbus2.h"


//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define  SYSCLK         24500000       // System clock frequency in Hz

#define  SMB_FREQUENCY  10000          // Target SCL clock rate
                                       // This example supports between 10kHz
                                       // and 100kHz

#define  WRITE          0x00           // SMBus WRITE command
#define  READ           0x01           // SMBus READ command

// Device addresses (7 bits, lsb is a don't care)
#define  SLAVE_ADDR     0x53           // Device address for slave target

// Status vector - top 4 bits only
#define  SMB_MTSTA      0xE0           // (MT) start transmitted
#define  SMB_MTDB       0xC0           // (MT) data byte transmitted
#define  SMB_MRDB       0x80           // (MR) data byte received
// End status vector definition

#define  NUM_BYTES_WR   1              // Number of bytes to write
                                       // Master -> Slave
#define  NUM_BYTES_RD   6              // Number of bytes to read
                                       // Master <- Slave

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

// Global holder for SMBus data
// All receive data is written here
unsigned char SMB_DATA_IN[NUM_BYTES_RD];

// Global holder for SMBus data.
// All transmit data is read from here
unsigned char SMB_DATA_OUT[NUM_BYTES_WR];


unsigned char TARGET;                  // Target SMBus slave address

bit SMB_BUSY;                          // Software flag to indicate when the
                                       // SMB_Read() or SMB_Write() functions
                                       // have claimed the SMBus

bit SMB_RW;                            // Software flag to indicate the
                                       // direction of the current transfer

unsigned long NUM_ERRORS;              // Counter for the number of errors.

// 16-bit SFR declarations
sfr16    TMR3RL   = 0x92;              // Timer3 reload registers
sfr16    TMR3     = 0x94;              // Timer3 counter registers

sbit SDA = P0^0;                       // SMBus on P0.0
sbit SCL = P0^1;                       // and P0.1

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SMBus_Init (void);

void SMBus_ISR (void);
void Timer3_ISR (void);


//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// SMBus configured as follows:
// - SMBus enabled
// - Slave mode inhibited
// - Timer1 used as clock source. The maximum SCL frequency will be
//   approximately 1/3 the Timer1 overflow rate
// - Setup and hold time extensions enabled
// - Bus Free and SCL Low timeout detection enabled
//
void SMBus_Init (void)
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

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave or arbitration states defined
// - All incoming data is written to global variable array <SMB_DATA_IN>
// - All outgoing data is read from global variable array <SMB_DATA_OUT>
//
void SMBus_ISR (void) interrupt 7
{
   bit FAIL = 0;                       // Used by the ISR to flag failed
                                       // transfers

   static unsigned char sent_byte_counter;
   static unsigned char rec_byte_counter;

   if (ARBLOST == 0)                   // Check for errors
   {
      // Normal operation
      switch (SMB0CN & 0xF0)           // Status vector
      {
         // Master Transmitter/Receiver: START condition transmitted.
         case SMB_MTSTA:
            SMB0DAT = TARGET;          // Load address of the target slave
            SMB0DAT &= 0xFE;           // Clear the LSB of the address for the
                                       // R/W bit
            SMB0DAT |= SMB_RW;         // Load R/W bit
            STA = 0;                   // Manually clear START bit
            rec_byte_counter = 1;      // Reset the counter
            sent_byte_counter = 1;     // Reset the counter
            break;

         // Master Transmitter: Data byte transmitted
         case SMB_MTDB:
            if (ACK)                   // Slave ACK?
            {
               if (SMB_RW == WRITE)    // If this transfer is a WRITE,
               {
                  if (sent_byte_counter <= NUM_BYTES_WR)
                  {
                     // send data byte
                     SMB0DAT = SMB_DATA_OUT[sent_byte_counter-1];
                     sent_byte_counter++;
                  }
                  else
                  {
                     STO = 1;          // Set STO to terminate transfer
                     SMB_BUSY = 0;     // And free SMBus interface
                  }
               }
               else {}                 // If this transfer is a READ,
                                       // proceed with transfer without
                                       // writing to SMB0DAT (switch
                                       // to receive mode)


            }
            else                       // If slave NACK,
            {
               STO = 1;                // Send STOP condition, followed
               STA = 1;                // By a START
               NUM_ERRORS++;           // Indicate error
            }
            break;

         // Master Receiver: byte received
         case SMB_MRDB:
            if (rec_byte_counter < NUM_BYTES_RD)
            {
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received
                                                          // byte
               ACK = 1;                // Send ACK to indicate byte received
               rec_byte_counter++;     // Increment the byte counter
            }
            else
            {
               SMB_DATA_IN[rec_byte_counter-1] = SMB0DAT; // Store received
                                                          // byte
               SMB_BUSY = 0;           // Free SMBus interface
               ACK = 0;                // Send NACK to indicate last byte
                                       // of this transfer

               STO = 1;                // Send STOP to terminate transfer
            }
            break;

         default:
            FAIL = 1;                  // Indicate failed transfer
                                       // and handle at end of ISR
            break;

      } // end switch
   }
   else
   {
      // ARBLOST = 1, error occurred... abort transmission
      FAIL = 1;
   } // end ARBLOST if

   if (FAIL)                           // If the transfer failed,
   {
      SMB0CF &= ~0x80;                 // Reset communication
      SMB0CF |= 0x80;
      STA = 0;
      STO = 0;
      ACK = 0;

      SMB_BUSY = 0;                    // Free SMBus

      FAIL = 0;
      LED = 0;

      NUM_ERRORS++;                    // Indicate an error occurred
   }

   SI = 0;                             // Clear interrupt flag
}

//-----------------------------------------------------------------------------
// Timer3 Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// A Timer3 interrupt indicates an SMBus SCL low timeout.
// The SMBus is disabled and re-enabled here
//
void Timer3_ISR (void) interrupt 14
{
   SMB0CF &= ~0x80;                    // Disable SMBus
   SMB0CF |= 0x80;                     // Re-enable SMBus
   TMR3CN &= ~0x80;                    // Clear Timer3 interrupt-pending
                                       // flag
   STA = 0;
   SMB_BUSY = 0;                       // Free SMBus
}

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SMB_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Writes a single byte to the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Write outgoing data to the <SMB_DATA_OUT> variable array
// 3) Call SMB_Write()
//
void SMB_Write (void)
{
   while (SMB_BUSY);                   // Wait for SMBus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 0;                         // Mark this transfer as a WRITE
   STA = 1;                            // Start transfer
}

//-----------------------------------------------------------------------------
// SMB_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Reads a single byte from the slave with address specified by the <TARGET>
// variable.
// Calling sequence:
// 1) Write target slave address to the <TARGET> variable
// 2) Call SMB_Write()
// 3) Read input data from <SMB_DATA_IN> variable array
//
void SMB_Read (void)
{
   while (SMB_BUSY);                   // Wait for bus to be free.
   SMB_BUSY = 1;                       // Claim SMBus (set to busy)
   SMB_RW = 1;                         // Mark this transfer as a READ

   STA = 1;                            // Start transfer

   while (SMB_BUSY);                   // Wait for transfer to complete
}



//-----------------------------------------------------------------------------
//  ACCELERO methods
//-----------------------------------------------------------------------------

volatile unsigned char dat;         // Test counter
volatile unsigned char data_count;  // SMB_DATA_IN and SMB_DATA_OUT counter
unsigned char i;                    // Dummy variable counters


void ACCE_begin()
{
   PCA0MD &= ~0x40;                    // WDTE = 0 (watchdog timer enable bit)

   OSCICN |= 0x03;                     // Set internal oscillator to highest
                                       // setting of 24500000

   // If slave is holding SDA low because of an improper SMBus reset or error
   while(!SDA)
   {
      // Provide clock pulses to allow the slave to advance out
      // of its current state. This will allow it to release SDA.
      XBR1 = 0x40;                     // Enable Crossbar
      SCL = 0;                         // Drive the clock low
      for(i = 0; i < 255; i++);        // Hold the clock low
      SCL = 1;                         // Release the clock
      while(!SCL);                     // Wait for open-drain
                                       // clock output to rise
      for(i = 0; i < 10; i++);         // Hold the clock high
      XBR1 = 0x00;                     // Disable Crossbar
   }

   SMBus_Init ();                      // Configure and enable SMBus

   EIE1 |= 0x01;                       // Enable the SMBus interrupt

   LED = 0;

   EA = 1;                             // Global interrupt enable

// TEST CODE-------------------------------------------------------------------

   dat = 0;                            // Output data counter
   NUM_ERRORS = 0;                     // Error counter
    SMBUS_start();

    // Power ON
    SMBUS_write_address(ACCE_ADDR, WRITE);
    SMBUS_write_value(POWER_CTRL);
    SMBUS_write_value(0x08); // Measurement Mode
}

void ACCE_read(
    unsigned char* ACCE_X0,
    unsigned char* ACCE_X1,
    unsigned char* ACCE_Y0,
    unsigned char* ACCE_Y1,
    unsigned char* ACCE_Z0,
    unsigned char* ACCE_Z1
)
{
   dat = 0;                            // Output data counter
   NUM_ERRORS = 0;                     // Error counter
   while (1)
   {
      // SMBus Write Sequence
      for (data_count = 0; data_count < NUM_BYTES_WR; data_count++)
      {
         SMB_DATA_OUT[data_count] = dat; // Define next outgoing byte
         dat++;
      }
      TARGET = SLAVE_ADDR;             // Target the Slave for next SMBus
                                       // transfer
      SMB_Write();                     // Initiate SMBus write

      // SMBus Read Sequence
      TARGET = SLAVE_ADDR;             // Target the Slave for next SMBus
                                       // transfer
      SMB_Read();

      // Check transfer data
      for (data_count = 0; data_count < NUM_BYTES_RD; data_count++)
      {
         // Received data match transmit data?
         if(SMB_DATA_IN[data_count] != SMB_DATA_OUT[data_count])
         {
            NUM_ERRORS++;              // Increment error counter if no
                                       // match
         }
      }

      // Run to here to view the SMB_DATA_IN and SMB_DATA_OUT variable arrays
      *ACCE_X0 = SMB_DATA_IN[0];
      *ACCE_X1 = SMB_DATA_IN[1];
      *ACCE_Y0 = SMB_DATA_IN[2];
      *ACCE_Y1 = SMB_DATA_IN[3];
      *ACCE_Z0 = SMB_DATA_IN[4];
      *ACCE_Z1 = SMB_DATA_IN[5];
      
      delay(1);
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------