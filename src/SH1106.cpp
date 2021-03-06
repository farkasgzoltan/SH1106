/*********************************************************************
This is a library for our Monochrome OLEDs based on SH1106 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

/*********************************************************************
I change the adafruit SH1106 to SH1106

SH1106 driver similar to SH1106 so, just change the display() method.

However, SH1106 driver don't provide several functions such as scroll commands.


*********************************************************************/
#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
//#error this library has been modified by @stickbreaker to increase i2c performance. It will only work with Arduino-Espressif 1.0.0-rc3 and later.

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
 #include <util/delay.h>
#endif
#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "SH1106.h"

typedef enum
{
    HORIZONTAL = 0,
    VERTICAL,
    PAGE,
    INVALID,
    END_MEMORY_ADDRESSING_MODES
} MEMORY_ADDRESSING_MODES;

// the memory buffer for the LCD
static uint8_t buffer[SH1106_LCDHEIGHT * SH1106_LCDWIDTH / 8] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SH1106_LCDHEIGHT * SH1106_LCDWIDTH > 96*16)
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xC0,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x40, 0x40, 0x60, 0x60, 0x60, 0x20,
0x30, 0x30, 0x30, 0x30, 0x38, 0x18, 0x18, 0x18, 0x1C, 0x1C, 0x1C, 0x1C, 0x0E, 0x0E, 0x0E, 0x0E,
0x0E, 0x0F, 0x0F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xDF, 0x9F,
0x9F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xE0, 0xE0, 0x00, 0x80, 0xF8, 0xFC, 0xF4, 0xF4, 0xF4, 0x06, 0x06, 0x06, 0x02, 0xC3,
0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0xF3, 0x03, 0x03, 0xC7, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
0x87, 0xC0, 0xF0, 0xFD, 0x7F, 0x3F, 0x3F, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SH1106_LCDHEIGHT == 64)
0x00, 0x00, 0x00, 0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x03, 0x01, 0x01, 0xC1, 0xC7,
0xC7, 0xC7, 0xC7, 0xC7, 0x87, 0xE0, 0xF8, 0xFE, 0xFF, 0xFF, 0xFF, 0xDF, 0xC7, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xC0, 0x00, 0x80, 0xBF, 0xBF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7D, 0x7D, 0xF9, 0xFB, 0xFB,
0xFB, 0xFB, 0xF3, 0x03, 0xC0, 0xF0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFC, 0x00, 0x80, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xF1, 0xF1, 0xF1, 0xFF, 0xFF,
0xFF, 0xBF, 0x3F, 0x07, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x81, 0xE0, 0xF0, 0xFC, 0xFF, 0xFF,
0xFF, 0xDF, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0x03, 0x01, 0x00, 0x10, 0x18, 0x0C, 0x06,
0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xBE, 0xBE, 0x9F, 0x9F, 0x9F,
0x9F, 0x0F, 0x1F, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x87, 0x87, 0x87, 0x07, 0x07, 0x1F, 0x1F,
0x1F, 0x1F, 0x1F, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x9F, 0xBE, 0x3E, 0x1E, 0x1E, 0x1F, 0x1F, 0x1F,
0x0F, 0x17, 0x1D, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 0x07, 0x07, 0x87, 0x87, 0x9F, 0x9F, 0x9F,
0x1F, 0x1F, 0x10, 0x1F, 0x9F, 0x9F, 0x9F, 0x9F, 0x83, 0x01, 0x01, 0x01, 0x1F, 0x1F, 0x1F, 0x1F,
0x1F, 0x0F, 0x00, 0x1F, 0x1F, 0x1F, 0x9F, 0x9F, 0x1F, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x07, 0x07,
0x07, 0x07, 0x07, 0x07, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x3F, 0x3F, 0x0F, 0x1D,
0x3F, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3F, 0x1F, 0x3F, 0x3E, 0x3C, 0x3E, 0x1E,
0x3E, 0x3E, 0x3C, 0x3E, 0x3E, 0x3E, 0x1E, 0x3F, 0x3F, 0x3F, 0x3E, 0x3E, 0x3E, 0x7E, 0xFE, 0xFE,
0xFE, 0x7E, 0x3E, 0xFE, 0xFE, 0x7E, 0x1E, 0x06, 0x00, 0x00, 0x3C, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x06,
0x06, 0x3E, 0x3E, 0x3E, 0x1E, 0x3E, 0x3F, 0x3F, 0x3E, 0x3E, 0x3E, 0x1E, 0x3E, 0x3E, 0x3E, 0x3E,
0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
};

#define sh1106_swap(a, b) { int16_t t = a; a = b; b = t; }

// the most basic function, set a single pixel
void SH1106::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    sh1106_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    sh1106_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // x is which column
    switch (color)
    {
      case WHITE:   buffer[x+ (y/8)*SH1106_LCDWIDTH] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x+ (y/8)*SH1106_LCDWIDTH] &= ~(1 << (y&7)); break;
      case INVERSE: buffer[x+ (y/8)*SH1106_LCDWIDTH] ^=  (1 << (y&7)); break;
    }

}

SH1106::SH1106(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS) :
  Adafruit_GFX(SH1106_LCDWIDTH, SH1106_LCDHEIGHT),
  cs(CS),
  rst(RST),
  dc(DC),
  sclk(SCLK),
  sid(SID),
  hwSPI(false)
{
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
SH1106::SH1106(int8_t DC, int8_t RST, int8_t CS) :
  Adafruit_GFX(SH1106_LCDWIDTH, SH1106_LCDHEIGHT),
  dc(DC),
  rst(RST),
  cs(CS),
  hwSPI(true)
{
}

// initializer for I2C - we only indicate the reset pin!
SH1106::SH1106(int8_t sda, int8_t scl) :
  Adafruit_GFX(SH1106_LCDWIDTH, SH1106_LCDHEIGHT),
  sda(sda),
  scl(scl),
  sclk(-1),
  sid(-1),
  dc(-1),
  rst(-1),
  cs(-1)
{
}

// initializer for I2C - we only indicate the reset pin!
SH1106::SH1106(int8_t reset) :
  Adafruit_GFX(SH1106_LCDWIDTH, SH1106_LCDHEIGHT),
  sda(SDA),
  scl(SCL),
  sclk(-1),
  sid(-1),
  dc(-1),
  rst(reset),
  cs(-1)
{
}


void SH1106::begin(uint8_t vccstate, uint8_t i2caddr, bool reset) {
  _vccstate = vccstate;
  _i2caddr = i2caddr;

  // set pin directions
  if (sid != -1){
    pinMode(dc, OUTPUT);
    pinMode(cs, OUTPUT);
#ifdef HAVE_PORTREG
    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);
    dcport      = portOutputRegister(digitalPinToPort(dc));
    dcpinmask   = digitalPinToBitMask(dc);
#endif
    if (!hwSPI){
      // set pins for software-SPI
      pinMode(sid, OUTPUT);
      pinMode(sclk, OUTPUT);
#ifdef HAVE_PORTREG
      clkport     = portOutputRegister(digitalPinToPort(sclk));
      clkpinmask  = digitalPinToBitMask(sclk);
      mosiport    = portOutputRegister(digitalPinToPort(sid));
      mosipinmask = digitalPinToBitMask(sid);
#endif
      }
    if (hwSPI){
      SPI.begin();
#ifdef SPI_HAS_TRANSACTION
      SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
#else
      SPI.setClockDivider (4);
#endif
    }
  }
  else
  {
    // I2C Init
    Wire.begin(sda, scl);
#ifdef __SAM3X8E__
    // Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
    TWI1->TWI_CWGR = 0;
    TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
#endif
    delay(100); //required to properly power on
  }
  if ((reset) && (rst >= 0)) {
    // Setup reset pin direction (used by both SPI and I2C)
    pinMode(rst, OUTPUT);
    digitalWrite(rst, HIGH);
    // VDD (3.3V) goes high at start, lets just chill for a ms
    delay(1);
    // bring reset low
    digitalWrite(rst, LOW);
    // wait 10ms
    delay(10);
    // bring out of reset
    digitalWrite(rst, HIGH);
    // turn on VCC (9V?)
  }

  // Init sequence
  SH1106_command(SH1106_DISPLAYOFF);                    // 0xAE
  SH1106_command(SH1106_SETDISPLAYCLOCKDIV);            // 0xD5

  #if defined SH1106_128_64
    // Init sequence for 128x64 OLED module
    SH1106_command(0xF0);                                 // the suggested ratio 0xF0 XXXXXXXX
    SH1106_command(SH1106_SETMULTIPLEX);                  // 0xA8 XXXXXXXXX
    SH1106_command(0x3F);								   // XXXXXXXXXXX
    SH1106_command(SH1106_OUTPUT_FOLLOWS_RAM);            // 0xA4 XXXXXXXXX
    SH1106_command(SH1106_SETDISPLAYOFFSET);              // 0xD3 XXXXXXXXX
    SH1106_command(0x0);                                  // no offset  XXXXXXXXX
    SH1106_command(SH1106_SETSTARTLINE | 0x0);            // line #0
    SH1106_command(SH1106_CHARGEPUMP);                    // 0x8D XXXXXXXXXX
    if (vccstate == SH1106_EXTERNALVCC)					   // XXXXXXXXX
      { SH1106_command(0x10); }						   // XXXXXXXXX
    else												   // XXXXXXXXX
      { SH1106_command(0x14); }						   // XXXXXXXXX
    SH1106_command(SH1106_MEMORYMODE);                    // 0x20 XXXXXXXXXXXXXX
//    SH1106_command(PAGE);                                  // 0x2 Paged XXXXXXXXXXX
    SH1106_command(HORIZONTAL);                                  // 0x0 Horizontal XXXXXXXXXXX
    SH1106_command(SH1106_SET_PAGE_ADDRESS); // start at page address 0 XXXXXXXXX
//    SH1106_command(SH1106_SEGREMAP | 0x1);				   // ?????????????
    SH1106_command(SH1106_COMSCANDEC);					   // XXXXXXXXXX
    SH1106_command(SH1106_SETLOWCOLUMN);				   // XXXXXXXXXX
    SH1106_command(SH1106_SETHIGHCOLUMN);				   // XXXXXXXXXX
    SH1106_command(SH1106_SETCOMPINS);                    // 0xDA XXXXXXXXX
    SH1106_command(0x12);								   // XXXXXXXXXX
    SH1106_command(SH1106_SETCONTRAST);                   // 0x81 XXXXXXXX
    if (vccstate == SH1106_EXTERNALVCC)					   // XXXXXXX
      { SH1106_command(0x9F); }						   // XXXXXXX
    else					   							   // XXXXXXX
      { SH1106_command(0xCF); }					       // XXXXXXX
    SH1106_command(SH1106_SET_SEGMENT_REMAP);             // 0xA1 XXXXXXXX
    SH1106_command(SH1106_SETPRECHARGE);                  // 0xd9 XXXXXXXXX
    if (vccstate == SH1106_EXTERNALVCC)					   // XXXXXXXXX
      { SH1106_command(0x22); }						   // XXXXXXXXX
    else												   // XXXXXXXXX
      { SH1106_command(0xF1); }						   // XXXXXXXXX
    SH1106_command(SH1106_SETVCOMDETECT);                 // 0xDB XXXXXXXX
    SH1106_command(0x20);								   // 0.77xVcc XXXXXXX
    SH1106_command(SH1106_DISPLAYALLON_RESUME);           // 0xA4 XXXXXXXXXX
    SH1106_command(SH1106_NORMALDISPLAY);                 // 0xA6  XXXXXXXXXX
  #endif

/*
  SH1106_command(0x80);                                  // the suggested ratio 0x80

  SH1106_command(SH1106_SETMULTIPLEX);                  // 0xA8
  SH1106_command(SH1106_LCDHEIGHT - 1);

  SH1106_command(SH1106_SETDISPLAYOFFSET);              // 0xD3
  SH1106_command(0x0);                                   // no offset
  SH1106_command(SH1106_SETSTARTLINE | 0x0);            // line #0
  SH1106_command(SH1106_CHARGEPUMP);                    // 0x8D
  if (vccstate == SH1106_EXTERNALVCC)
    { SH1106_command(0x10); }
  else
    { SH1106_command(0x14); }
  SH1106_command(SH1106_MEMORYMODE);                    // 0x20
  SH1106_command(0x00);                                  // 0x0 act like ks0108
  SH1106_command(SH1106_SEGREMAP | 0x1);
  SH1106_command(SH1106_COMSCANDEC);

 #if defined SH1106_128_32
  SH1106_command(SH1106_SETCOMPINS);                    // 0xDA
  SH1106_command(0x02);
  SH1106_command(SH1106_SETCONTRAST);                   // 0x81
  SH1106_command(0x8F);

#elif defined SH1106_128_64
  SH1106_command(SH1106_SETCOMPINS);                    // 0xDA
  SH1106_command(0x12);
  SH1106_command(SH1106_SETCONTRAST);                   // 0x81
  if (vccstate == SH1106_EXTERNALVCC)
    { SH1106_command(0x9F); }
  else
    { SH1106_command(0xCF); }

#elif defined SH1106_96_16
  SH1106_command(SH1106_SETCOMPINS);                    // 0xDA
  SH1106_command(0x2);   //ada x12
  SH1106_command(SH1106_SETCONTRAST);                   // 0x81
  if (vccstate == SH1106_EXTERNALVCC)
    { SH1106_command(0x10); }
  else
    { SH1106_command(0xAF); }

#endif

  SH1106_command(SH1106_SETPRECHARGE);                  // 0xd9
  if (vccstate == SH1106_EXTERNALVCC)
    { SH1106_command(0x22); }
  else
    { SH1106_command(0xF1); }
  SH1106_command(SH1106_SETVCOMDETECT);                 // 0xDB
  SH1106_command(0x40);
  SH1106_command(SH1106_DISPLAYALLON_RESUME);           // 0xA4
  SH1106_command(SH1106_NORMALDISPLAY);                 // 0xA6

  SH1106_command(SH1106_DEACTIVATE_SCROLL);
*/
  SH1106_command(SH1106_DISPLAYON);//--turn on oled panel
}

void SH1106::flipScreenVertically() {
  SH1106_command(0xA0);              //SEGREMAP   //Rotate screen 180 deg
  SH1106_command(SH1106_SETCOMPINS);
  SH1106_command(0x22);
  SH1106_command(SH1106_COMSCANINC); //COMSCANDEC  Rotate screen 180 Deg
}

void SH1106::invertDisplay(uint8_t i) {
  if (i) {
    SH1106_command(SH1106_INVERTDISPLAY);
  } else {
    SH1106_command(SH1106_NORMALDISPLAY);
  }
}

void SH1106::SH1106_command(uint8_t c) {
  if (sid != -1)
  {
    // SPI
#ifdef HAVE_PORTREG
    *csport |= cspinmask;
    *dcport &= ~dcpinmask;
    *csport &= ~cspinmask;
#else
    digitalWrite(cs, HIGH);
    digitalWrite(dc, LOW);
    digitalWrite(cs, LOW);
#endif
    fastSPIwrite(c);
#ifdef HAVE_PORTREG
    *csport |= cspinmask;
#else
    digitalWrite(cs, HIGH);
#endif
  }
  else
  {
    // I2C
    uint32_t timeout=millis();
    do{
      uint8_t control = 0x00;   // Co = 0, D/C = 0
      Wire.beginTransmission(_i2caddr);
      Wire.write(control);
      Wire.write(c);
      if(Wire.endTransmission()!=0){
        Serial.print(".");
//        log_e("cmd Failure=%d (%s)",Wire.lastError(),Wire.getErrorText(Wire.lastError()));
      }
    }while((millis()-timeout<100)&&(Wire.lastError()!=0));    
  }
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
/*void SH1106::startscrollright(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_RIGHT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X00);
  SH1106_command(0XFF);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SH1106::startscrollleft(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_LEFT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X00);
  SH1106_command(0XFF);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SH1106::startscrolldiagright(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_SET_VERTICAL_SCROLL_AREA);
  SH1106_command(0X00);
  SH1106_command(SH1106_LCDHEIGHT);
  SH1106_command(SH1106_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X01);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SH1106::startscrolldiagleft(uint8_t start, uint8_t stop){
  SH1106_command(SH1106_SET_VERTICAL_SCROLL_AREA);
  SH1106_command(0X00);
  SH1106_command(SH1106_LCDHEIGHT);
  SH1106_command(SH1106_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  SH1106_command(0X00);
  SH1106_command(start);
  SH1106_command(0X00);
  SH1106_command(stop);
  SH1106_command(0X01);
  SH1106_command(SH1106_ACTIVATE_SCROLL);
}

void SH1106::stopscroll(void){
  SH1106_command(SH1106_DEACTIVATE_SCROLL);
}
*/

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void SH1106::dim(boolean dim) {
  uint8_t contrast;

  if (dim) {
    contrast = 0; // Dimmed display
  } else {
    if (_vccstate == SH1106_EXTERNALVCC) {
      contrast = 0x9F;
    } else {
      contrast = 0xCF;
    }
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  SH1106_command(SH1106_SETCONTRAST);
  SH1106_command(contrast);
}

void SH1106::setContrast(uint8_t contrast) {
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  SH1106_command(SH1106_SETCONTRAST);
  SH1106_command(contrast);
}

/*#define SH1106_SETLOWCOLUMN 0x00
#define SH1106_SETHIGHCOLUMN 0x10
#define SH1106_SETSTARTLINE 0x40*/

void SH1106::display(void) {

    int k = 0;
    for (int page = 0; page < SH1106_MAX_PAGE_COUNT; page++)
    {
        SH1106_command(SH1106_SET_PAGE_ADDRESS + page);
        SH1106_command(0x02); // low column start address
        SH1106_command(0x10); // high column start address

		if (sid != -1)
		{
			 for (int pixel = 0; pixel < SH1106_LCDWIDTH; pixel++)
			 {	
				// SPI
				#ifdef HAVE_PORTREG
					*csport |= cspinmask;
					*dcport |= dcpinmask;
					*csport &= ~cspinmask;
				#else
					digitalWrite(cs, HIGH);
					digitalWrite(dc, HIGH);
					digitalWrite(cs, LOW);
				#endif

				fastSPIwrite(buffer[(page << 7) + pixel]);  // Page * 128 + pixel

				#ifdef HAVE_PORTREG
					*csport |= cspinmask;
				#else
					digitalWrite(cs, HIGH);
				#endif
				
			}
		}
		else
		{
				// save I2C bitrate
// save I2C bitrate
			#ifdef TWBR
				uint8_t twbrbackup = TWBR;
				TWBR = 12; // upgrade to 400KHz!
			#endif

			//Serial.println(TWBR, DEC);
			//Serial.println(TWSR & 0x3, DEC);

			// I2C
			for (uint16_t i=0; i<8; i++) {
				// send a bunch of data in one xmission
				Wire.beginTransmission(_i2caddr);
				Wire.write(0x40);
				for (uint8_t x=0; x<16; x++) {
				//for (uint8_t x=0; x<16; x++, k++) {
					Wire.write(buffer[k]);
					k++;
				}
				  
				Wire.endTransmission();
			}
			#ifdef TWBR
				TWBR = twbrbackup;
			#endif
		}
	}
}

void SH1106::clearDisplay(void) {
  memset(buffer, 0, (SH1106_LCDWIDTH*SH1106_LCDHEIGHT/8));
}


inline void SH1106::fastSPIwrite(uint8_t d) {
  if(hwSPI) {
    (void)SPI.transfer(d);
  } else {
    shiftOut(sid, sclk, MSBFIRST, d);		// SSD1306 specs show MSB out first
  }
}

void SH1106::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  boolean bSwap = false;
  switch(rotation) {
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      sh1106_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      sh1106_swap(x, y);
      y = HEIGHT - y - 1;
      y -= (w-1);
      break;
  }

  if(bSwap) {
    drawFastVLineInternal(x, y, w, color);
  } else {
    drawFastHLineInternal(x, y, w, color);
  }
}

void SH1106::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Do bounds/limit checks
  if(y < 0 || y >= HEIGHT) { return; }

  // make sure we don't try to draw below 0
  if(x < 0) {
    w += x;
    x = 0;
  }

  // make sure we don't go off the edge of the display
  if( (x + w) > WIDTH) {
    w = (WIDTH - x);
  }

  // if our width is now negative, punt
  if(w <= 0) { return; }

  // set up the pointer for  movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SH1106_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  register uint8_t mask = 1 << (y&7);

  switch (color)
  {
  case WHITE:         while(w--) { *pBuf++ |= mask; }; break;
    case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
  case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
  }
}

void SH1106::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  bool bSwap = false;
  switch(rotation) {
    case 0:
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
      bSwap = true;
      sh1106_swap(x, y);
      x = WIDTH - x - 1;
      x -= (h-1);
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      y -= (h-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y
      bSwap = true;
      sh1106_swap(x, y);
      y = HEIGHT - y - 1;
      break;
  }

  if(bSwap) {
    drawFastHLineInternal(x, y, h, color);
  } else {
    drawFastVLineInternal(x, y, h, color);
  }
}


void SH1106::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= WIDTH) { return; }

  // make sure we don't try to draw below 0
  if(__y < 0) {
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;

  }

  // make sure we don't go past the height of the display
  if( (__y + __h) > HEIGHT) {
    __h = (HEIGHT - __y);
  }

  // if our height is now negative, punt
  if(__h <= 0) {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t y = __y;
  register uint8_t h = __h;


  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SH1106_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if(mod) {
    // mask off the high n bits we want to set
    mod = 8-mod;

    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // register uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if( h < mod) {
      mask &= (0XFF >> (mod-h));
    }

  switch (color)
    {
    case WHITE:   *pBuf |=  mask;  break;
    case BLACK:   *pBuf &= ~mask;  break;
    case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h<mod) { return; }

    h -= mod;

    pBuf += SH1106_LCDWIDTH;
  }


  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8) {
    if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
      do  {
      *pBuf=~(*pBuf);

        // adjust the buffer forward 8 rows worth of data
        pBuf += SH1106_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    else {
      // store a local value to work with
      register uint8_t val = (color == WHITE) ? 255 : 0;

      do  {
        // write our value in
      *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += SH1106_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    }

  // now do the final partial byte, if necessary
  if(h) {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // register uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    register uint8_t mask = postmask[mod];
    switch (color)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }
  }
}
