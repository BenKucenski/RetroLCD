/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
/**
   Bare minimum i2c driver for OLED display
   bkucenski@gmail.com 2/22/2019
   based on https://github.com/adafruit/Adafruit_SSD1306
*/
#ifndef DISPLAY128X64_H
#define DISPLAY128X64_H

#include <Wire.h>

#define CLOCK_DISPLAY 400000 // this seems to work.  Faster is better

#define SSD1306_MEMORYMODE          0x20 ///< See datasheet
#define SSD1306_COLUMNADDR          0x21 ///< See datasheet
#define SSD1306_PAGEADDR            0x22 ///< See datasheet
#define SSD1306_SETCONTRAST         0x81 ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8D ///< See datasheet
#define SSD1306_SEGREMAP            0xA0 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5 ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6 ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7 ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8 ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< See datasheet
#define SSD1306_DISPLAYON           0xAF ///< See datasheet
#define SSD1306_COMSCANINC          0xC0 ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range

#define SCREEN_ADDR 0x3C
#define SCREEN_WIDTH_BYTES 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_BYTES 1024

#define SCREEN_LANDSCAPE 0
#define SCREEN_PORTRAIT 1


#define byte uint8_t

#if defined(BUFFER_LENGTH)
#define WIRE_MAX BUFFER_LENGTH          ///< AVR or similar Wire lib
#elif defined(SERIAL_BUFFER_SIZE)
#define WIRE_MAX (SERIAL_BUFFER_SIZE-1) ///< Newer Wire uses RingBuffer
#else
#define WIRE_MAX 32                     ///< Use common Arduino core default
#endif

class Display128x64 {
  private:
    static byte screen[SCREEN_BYTES];
    static int ix, iy, iz;
    static byte b, m;

    static void SendCommand(uint8_t c);
    static void SendCommands(const uint8_t *c, uint8_t n);

  public:
    static byte Rotation;

    static void ClearDisplay();
    static bool InitDisplay(uint8_t vccstate);
    static void PushToDisplay();

    static void PlotPixel(int x, int y, byte c);
    static void DrawLine(int x1, int y1, int x2, int y2, byte c);
    static void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, byte c);

    static void Blit(int x, int y, const byte * img); // 16x16 sprites
    static void BlitMask(int x, int y, const byte * img, const byte * msk);// 16x16 sprites with mask
    static void Blit8(int x, int y, const byte * img); // 8x8 sprites
    static void BlitMask8(int x, int y, const byte * img, const byte * msk); // 8x8 sprites with mask
    static void BlitMask8FlipH(int x, int y, const byte * img, const byte * msk); // 8x8 sprites with mask flipped horizontally
    static void Blit816(int x, int y, const byte * img); // 8x16 sprites
    static void BlitMask816(int x, int y, const byte * img, const byte * msk); // 8x16 sprites with mask
    static void Blit58(int x, int y, const byte * img); // 5x8 sprites
    static void BlitMask58(int x, int y, const byte * img, const byte * msk); // 5x8 sprites with mask

    static void BlitText58(int x, int y, const byte * font, char * str); // 5x8 font - LCD1602
    static void BlitText58N(int x, int y, const byte * font, char * str, int n); // n is the spacing of the font
    static void BlitText816(int x, int y, const byte * font, char * str); // 8x16 font - Codepage437.h

};









#endif
