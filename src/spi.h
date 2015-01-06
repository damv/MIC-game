#ifndef __SPI_H__
#define __SPI_H__

#define HX8340B_LCDWIDTH                  176
#define HX8340B_LCDHEIGHT                 220

// HX8340-B(N) Commands (used by BTL221722-276L)
#define HX8340B_N_NOP                     (0x00)
#define HX8340B_N_SWRESET                 (0x01)
#define HX8340B_N_RDDIDIF                 (0x04)
#define HX8340B_N_RDDST                   (0x09)
#define HX8340B_N_RDDPM                   (0x0A)
#define HX8340B_N_RDDMADCTL               (0x0B)
#define HX8340B_N_RDDCOLMOD               (0x0C)
#define HX8340B_N_RDDIM                   (0x0D)
#define HX8340B_N_RDDSM                   (0x0E)
#define HX8340B_N_RDDSDR                  (0x0F)
#define HX8340B_N_SLPIN                   (0x10)
#define HX8340B_N_SPLOUT                  (0x11)
#define HX8340B_N_PTLON                   (0x12)
#define HX8340B_N_NORON                   (0x13)
#define HX8340B_N_INVOFF                  (0x20)
#define HX8340B_N_INVON                   (0x21)
#define HX8340B_N_GAMSET                  (0x26)
#define HX8340B_N_DISPOFF                 (0x28)
#define HX8340B_N_DISPON                  (0x29)
#define HX8340B_N_CASET                   (0x2A)
#define HX8340B_N_PASET                   (0x2B)
#define HX8340B_N_RAMWR                   (0x2C)
#define HX8340B_N_RAMRD                   (0x2E)
#define HX8340B_N_RGBSET                  (0x2D)
#define HX8340B_N_PLTAR                   (0x30)
#define HX8340B_N_VSCRDEF                 (0x33)
#define HX8340B_N_TEOFF                   (0x34)
#define HX8340B_N_TEON                    (0x35)
#define HX8340B_N_MADCTL                  (0x36)
#define HX8340B_N_VSCRSADD                (0x37)
#define HX8340B_N_IDMOFF                  (0x38)
#define HX8340B_N_IDMON                   (0x39)
#define HX8340B_N_COLMOD                  (0x3A)
#define HX8340B_N_RDID1                   (0xDA)
#define HX8340B_N_RDID2                   (0xDB)
#define HX8340B_N_RDID3                   (0xDC)
#define HX8340B_N_SETOSC                  (0xB0)
#define HX8340B_N_SETPWCTR1               (0xB1)
#define HX8340B_N_SETPWCTR2               (0xB2)
#define HX8340B_N_SETPWCTR3               (0xB3)
#define HX8340B_N_SETPWCTR4               (0xB4)
#define HX8340B_N_SETPWCTR5               (0xB5)
#define HX8340B_N_SETDISCTRL              (0xB6)
#define HX8340B_N_SETFRMCTRL              (0xB7)
#define HX8340B_N_SETDISCYCCTRL           (0xB8)
#define HX8340B_N_SETINVCTRL              (0xB9)
#define HX8340B_N_RGBBPCTR                (0xBA)
#define HX8340B_N_SETRGBIF                (0xBB)
#define HX8340B_N_SETDODC                 (0xBC)
#define HX8340B_N_SETINTMODE              (0xBD)
#define HX8340B_N_SETPANEL                (0xBE)
#define HX8340B_N_SETOTP                  (0xC7)
#define HX8340B_N_SETONOFF                (0xC0)
#define HX8340B_N_SETEXTCMD               (0xC1)
#define HX8340B_N_SETGAMMAP               (0xC2)
#define HX8340B_N_SETGAMMAN               (0xC3)

#define CS_ENABLE 0
#define CS_DISABLE 1

// screen control pins
sbit SCREEN_CS = P1^5;
sbit SCREEN_RST = P1^6;

sbit SPI_MOSI = P2^0;
sbit SPI_MISO = P2^1;
sbit SPI_SCK = P2^2;

void SPI_init();
void SPI_writeCommand(unsigned short c);
void SPI_writeData(unsigned short c);

void screen_init();
void screen_fill(unsigned short color);
void screen_setWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void screen_drawPixel(unsigned short x, unsigned short y, unsigned short color);
void screen_drawNumber(unsigned short x, unsigned short y, unsigned char num, unsigned short color, unsigned short bgcolor);
void screen_drawFastVLine(unsigned short x, unsigned short y, unsigned short h, unsigned short color);


#endif
