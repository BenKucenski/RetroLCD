/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include <Wire.h>
#include "Display128x64.h"

byte Display128x64::screen[SCREEN_BYTES];
int Display128x64::ix, Display128x64::iy, Display128x64::iz;
byte Display128x64::b, Display128x64::m, Display128x64::Rotation;

void Display128x64::PlotPixel(int x, int y, byte c)
{
  switch (Rotation) {
    case SCREEN_PORTRAIT:
      iz = x;
      x = y;
      y = iz;
      y = 63 - y;
      break;
  }

  if (y < 0) {
    return;
  }
  if (y >= SCREEN_HEIGHT) {
    return;
  }
  if (x < 0) {
    return;
  }
  if (x >= SCREEN_WIDTH) {
    return;
  }

  if (c) {
    screen[x + (y / 8) * SCREEN_WIDTH] |= (1 << (y & 7));
  } else {
    screen[x  + (y / 8) * SCREEN_WIDTH] &= ~(1 << (y & 7));
  }
}

// https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/LastVersion/olcPixelGameEngine.h
void Display128x64::DrawLine(int x1, int y1, int x2, int y2, byte c)
{
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

  dx = x2 - x1;
  dy = y2 - y1;

  if (dx == 0) // Line is vertical
  {
    if (y2 < y1) {
      int t = y2;
      y2 = y1;
      y1 = t;
    }
    for (y = y1; y <= y2; y++) {
      PlotPixel(x1, y, c);
    }
    return;
  }

  if (dy == 0) // Line is horizontal
  {
    if (x2 < x1) {
      int t = x2;
      x2 = x1;
      x1 = t;
    }
    for (x = x1; x <= x2; x++) {
      PlotPixel(x, y1, c);
    }
    return;
  }
  dx1 = abs(dx);
  dy1 = abs(dy);
  px = 2 * dy1 - dx1;
  py = 2 * dx1 - dy1;

  if (dy1 <= dx1)
  {
    if (dx >= 0)
    {
      x = x1;
      y = y1;
      xe = x2;
    }
    else
    {
      x = x2;
      y = y2;
      xe = x1;
    }

    PlotPixel(x, y, c);

    for (i = 0; x < xe; i++)
    {
      x = x + 1;
      if (px < 0) {
        px = px + 2 * dy1;
      }
      else
      {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          y = y + 1;
        } else {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      PlotPixel(x, y, c);
    }
  }
  else
  {
    if (dy >= 0)
    {
      x = x1;
      y = y1;
      ye = y2;
    }
    else
    {
      x = x2;
      y = y2;
      ye = y1;
    }

    PlotPixel(x, y, c);

    for (i = 0; y < ye; i++)
    {
      y = y + 1;
      if (py <= 0) {
        py = py + 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          x = x + 1;
        } else {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      PlotPixel(x, y, c);
    }
  }
}

void Display128x64::DrawDashedLine(int x1, int y1, int x2, int y2, byte c, byte n)
{
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  byte pixel_count = 0;
  byte pixel_color = c;
  
  dx = x2 - x1;
  dy = y2 - y1;

  if (dx == 0) // Line is vertical
  {
    if (y2 < y1) {
      int t = y2;
      y2 = y1;
      y1 = t;
    }
    for (y = y1; y <= y2; y++) {
      PlotPixel(x1, y, pixel_color);
      pixel_count++;
      if(pixel_count % n == 0) {
        pixel_count = 0;
        pixel_color = 1 - pixel_color;
      }
    }
    return;
  }

  if (dy == 0) // Line is horizontal
  {
    if (x2 < x1) {
      int t = x2;
      x2 = x1;
      x1 = t;
    }
    for (x = x1; x <= x2; x++) {
      PlotPixel(x, y1, pixel_color);
      pixel_count++;
      if(pixel_count % n == 0) {
        pixel_count = 0;
        pixel_color = 1 - pixel_color;
      }
    }
    return;
  }
  dx1 = abs(dx);
  dy1 = abs(dy);
  px = 2 * dy1 - dx1;
  py = 2 * dx1 - dy1;

  if (dy1 <= dx1)
  {
    if (dx >= 0)
    {
      x = x1;
      y = y1;
      xe = x2;
    }
    else
    {
      x = x2;
      y = y2;
      xe = x1;
    }

    PlotPixel(x, y, pixel_color);
    pixel_count++;
    if(pixel_count % n == 0) {
      pixel_count = 0;
      pixel_color = 1 - pixel_color;
    }

    for (i = 0; x < xe; i++)
    {
      x = x + 1;
      if (px < 0) {
        px = px + 2 * dy1;
      }
      else
      {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          y = y + 1;
        } else {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      PlotPixel(x, y, pixel_color);
      pixel_count++;
      if(pixel_count % n == 0) {
        pixel_count = 0;
        pixel_color = 1 - pixel_color;
      }
    }
  }
  else
  {
    if (dy >= 0)
    {
      x = x1;
      y = y1;
      ye = y2;
    }
    else
    {
      x = x2;
      y = y2;
      ye = y1;
    }

    PlotPixel(x, y, pixel_color);
    pixel_count++;
    if(pixel_count % n == 0) {
      pixel_count = 0;
      pixel_color = 1 - pixel_color;
    }

    for (i = 0; y < ye; i++)
    {
      y = y + 1;
      if (py <= 0) {
        py = py + 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          x = x + 1;
        } else {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      PlotPixel(x, y, pixel_color);
      pixel_count++;
      if(pixel_count % n == 0) {
        pixel_count = 0;
        pixel_color = 1 - pixel_color;
      }
    }
  }
}

void Display128x64::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, byte c)
{
  DrawLine(x1, y1, x2, y2, c);
  DrawLine(x1, y1, x3, y3, c);
  DrawLine(x2, y2, x3, y3, c);
}

// 16x16 sprites
void Display128x64::Blit(int x, int y, const byte * img)
{
  for (iy = 0; iy < 16; iy++) {
    b = pgm_read_byte(img + iy * 2);
    for (ix = 0; ix < 8; ix++) {
      PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
    }
    b = pgm_read_byte(img + iy * 2 + 1);
    for (ix = 8; ix < 16; ix++) {
      PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
    }
  }
}

// 16x16 sprites with mask
void Display128x64::BlitMask(int x, int y, const byte * img, const byte * msk)
{
  for (iy = 0; iy < 16; iy++) {
    b = pgm_read_byte(img + iy * 2);
    m = pgm_read_byte(img + iy * 2);
    for (ix = 0; ix < 8; ix++) {
      if ((byte)((m >> (7 - (ix % 8))) & 1)) {
        PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
      }
    }

    b = pgm_read_byte(img + iy * 2 + 1);
    m = pgm_read_byte(img + iy * 2 + 1);
    for (ix = 8; ix < 16; ix++) {
      if ((byte)((m >> (7 - (ix % 8))) & 1)) {
        PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
      }
    }
  }
}

// 8x8 sprites
void Display128x64::Blit8(int x, int y, const byte * img)
{
  for (iy = 0; iy < 8; iy++) {
    b = pgm_read_byte(img + iy);
    for (ix = 0; ix < 8; ix++) {
      PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
    }
  }
}

// 8x8 sprites with FlipH
void Display128x64::Blit8FlipH(int x, int y, const byte * img)
{
  for (iy = 0; iy < 8; iy++) {
    b = pgm_read_byte(img + iy);
    for (ix = 0; ix < 8; ix++) {
      PlotPixel(x + ix, y + iy, (byte)((b >> (ix % 8)) & 1));
    }
  }
}

// 8x8 sprites with mask
void Display128x64::BlitMask8(int x, int y, const byte * img, const byte * msk)
{
  for (iy = 0; iy < 8; iy++) {
    b = pgm_read_byte(img + iy);
    m = pgm_read_byte(img + iy);
    for (ix = 0; ix < 8; ix++) {
      if ((byte)((m >> (7 - (ix % 8))) & 1)) {
        PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
      }
    }
  }
}

// 8x8 sprites with mask FlipH
void Display128x64::BlitMask8FlipH(int x, int y, const byte * img, const byte * msk)
{
  for (iy = 0; iy < 8; iy++) {
    b = pgm_read_byte(img + iy);
    m = pgm_read_byte(img + iy);
    for (ix = 0; ix < 8; ix++) {
      if ((byte)((m >> (ix % 8)) & 1)) {
        PlotPixel(x + ix, y + iy, (byte)((b >> (ix % 8)) & 1));
      }
    }
  }
}

// 8x16 sprites
void Display128x64::Blit816(int x, int y, const byte * img)
{
  for (iy = 0; iy < 16; iy++) {
    b = pgm_read_byte(img + iy);
    for (ix = 0; ix < 8; ix++) {
      PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
    }
  }
}

// 8x16 sprites with mask
void Display128x64::BlitMask816(int x, int y, const byte * img, const byte * msk)
{
  for (iy = 0; iy < 16; iy++) {
    b = pgm_read_byte(img + iy);
    m = pgm_read_byte(img + iy);
    for (ix = 0; ix < 8; ix++) {
      if ((byte)((m >> (7 - (ix % 8))) & 1)) {
        PlotPixel(x + ix, y + iy, (byte)((b >> (7 - (ix % 8))) & 1));
      }
    }
  }
}


// 5x8 sprites
void Display128x64::Blit58(int x, int y, const byte * img)
{
  for (ix = 0; ix < 5; ix++) {
    b = pgm_read_byte(img + ix);
    for (iy = 0; iy < 8; iy++) {
      PlotPixel(x + ix, y + iy, (byte)((b >> ((iy % 8))) & 1));
    }
  }
}

// 5x8 sprites with mask
void Display128x64::BlitMask58(int x, int y, const byte * img, const byte * msk)
{
  for (ix = 0; ix < 5; ix++) {
    b = pgm_read_byte(img + ix);
    m = pgm_read_byte(msk + ix);
    for (iy = 0; iy < 8; iy++) {
      if ((byte)((m >> ((iy % 8))) & 1)) {
        PlotPixel(x + ix, y + iy, (byte)((b >> ((iy % 8))) & 1));
      }
    }
  }
}

void Display128x64::BlitText58(int x, int y, const byte * font, char * str)
{
  int org_x = x;
  while (str[0] && x < 128) {
    if (str[0] == 10) { // 10 = 0x0a \n, 13 = 0x0d \r
      x = org_x;
      y += 8;
      str++;
      continue;
    }
    Blit58(x, y, font + 5 * str[0]);
    str++;
    x += 5;
  }
}

void Display128x64::BlitText58N(int x, int y, const byte * font, char * str, int n)
{
  while (str[0] && x < 128) {
    Blit58(x, y, font + 5 * str[0]);
    str++;
    x += 5 + n;
  }
}

void Display128x64::BlitText816(int x, int y, const byte * font, char * str)
{
  while (str[0] && x < 128) {
    Blit816(x, y, font + 16 * str[0]);
    str++;
    x += 8;
  }
}

/////////////////////////////////////
//
// Functions and data to initialize
// and send data to the display
//
/////////////////////////////////////


static const uint8_t PROGMEM init1[] = {
  SSD1306_DISPLAYOFF,                   // 0xAE
  SSD1306_SETDISPLAYCLOCKDIV,           // 0xD5
  0x80,                                 // the suggested ratio 0x80
  SSD1306_SETMULTIPLEX
};               // 0xA8

static const uint8_t PROGMEM init2[] = {
  SSD1306_SETDISPLAYOFFSET,             // 0xD3
  0x0,                                  // no offset
  SSD1306_SETSTARTLINE | 0x0,           // line #0
  SSD1306_CHARGEPUMP
};                 // 0x8D

static const uint8_t PROGMEM init3[] = {
  SSD1306_MEMORYMODE,                   // 0x20
  0x00,                                 // 0x0 act like ks0108
  SSD1306_SEGREMAP | 0x1,
  SSD1306_COMSCANDEC
};

// 128x64 display
static const uint8_t PROGMEM init4b[] = {
  SSD1306_SETCOMPINS,                 // 0xDA
  0x12,
  SSD1306_SETCONTRAST
};              // 0x81

static const uint8_t PROGMEM init5[] = {
  SSD1306_SETVCOMDETECT,               // 0xDB
  0x40,
  SSD1306_DISPLAYALLON_RESUME,         // 0xA4
  SSD1306_NORMALDISPLAY,               // 0xA6
  SSD1306_DEACTIVATE_SCROLL,
  SSD1306_DISPLAYON
};                 // Main screen turn on

static const uint8_t PROGMEM dlist1[] = {
  SSD1306_PAGEADDR,
  0,                         // Page start address
  0xFF,                      // Page end (not really, but works here)
  SSD1306_COLUMNADDR,
  0
};                       // Column start address

void Display128x64::ClearDisplay()
{
  memset(screen, 0, SCREEN_BYTES);
}

void Display128x64::SendCommand(uint8_t c)
{
  Wire.beginTransmission(SCREEN_ADDR);
  Wire.write((uint8_t)0x00); // Co = 0, D/C = 0
  Wire.write(c);
  Wire.endTransmission();
}

void Display128x64::SendCommands(const uint8_t *c, uint8_t n)
{
  Wire.beginTransmission(SCREEN_ADDR);
  Wire.write((uint8_t)0x00); // Co = 0, D/C = 0
  uint8_t bytesOut = 1;
  while (n--) {
    if (bytesOut >= WIRE_MAX) {
      Wire.endTransmission();
      Wire.beginTransmission(SCREEN_ADDR);
      Wire.write((uint8_t)0x00); // Co = 0, D/C = 0
      bytesOut = 1;
    }
    Wire.write(pgm_read_byte(c++));
    bytesOut++;
  }
  Wire.endTransmission();
}

bool Display128x64::InitDisplay(uint8_t vccstate)
{
  if ((SCREEN_WIDTH != 128) && (SCREEN_HEIGHT != 64)) {
    return false;
  }

  ClearDisplay();

  // Init sequence
  SendCommands(init1, sizeof(init1));
  SendCommand(SCREEN_HEIGHT - 1);

  SendCommands(init2, sizeof(init2));
  SendCommand((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

  SendCommands(init3, sizeof(init3));

  SendCommands(init4b, sizeof(init4b));
  SendCommand((vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF);

  SendCommand(SSD1306_SETPRECHARGE); // 0xd9
  SendCommand((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
  SendCommands(init5, sizeof(init5));

  return true; // Success
}


void Display128x64::PushToDisplay()
{
  SendCommands(dlist1, sizeof(dlist1));
  SendCommand(SCREEN_WIDTH - 1); // Column end address

  uint8_t *ptr   = screen;
  uint16_t count = SCREEN_BYTES;

  Wire.beginTransmission(SCREEN_ADDR);
  Wire.write((uint8_t)0x40);
  uint8_t bytesOut = 1;
  while (count--) {
    if (bytesOut >= WIRE_MAX) {
      Wire.endTransmission();
      Wire.beginTransmission(SCREEN_ADDR);
      Wire.write((uint8_t)0x40);
      bytesOut = 1;
    }
    Wire.write(*ptr++);
    bytesOut++;
  }
  Wire.endTransmission();
}
