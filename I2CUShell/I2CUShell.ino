/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include <Arduino.h>
#include <Wire.h>
#include "Nunchuk.h"
#include "Display128x64.h"
#include "LCD1602.h"

// graphics stuff
byte CurrentFrame;
char StringBuffer[32];

// input stuff
float InputForceX;
float InputForceY;
byte InputC; // can be held down
byte InputZ;
byte InputCAlt; // can't be held down
byte InputZAlt;


void setup() {
  Wire.begin();

  Nunchuk::NunchukInit();
  Nunchuk::NunchukRead();
  delay(100);
  

  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  CurrentFrame = 0;
  randomSeed(analogRead(0));
}


void ReadController()
{
  Wire.setClock(CLOCK_NUNCHUK);

  if (!Nunchuk::NunchukRead()) {
    return;
  }

  InputForceX = map(Nunchuk::NunchukX(), -128, 127, 0, 255) * 1.0 / 128.0 - 1.0;
  InputForceY = (255 - map(Nunchuk::NunchukY(), 127, -128, 0, 255)) * 1.0 / 128.0 - 1.0;
  InputC = Nunchuk::NunchukC();
  InputZ = Nunchuk::NunchukZ();
  InputCAlt = Nunchuk::NunchukCAlt();
  InputZAlt = Nunchuk::NunchukZAlt();
}



void RenderDisplay()
{
  Display128x64::ClearDisplay();

  sprintf(StringBuffer, "RetroLCD.com");
  Display128x64::BlitText58N(0, 5, LCD1602, StringBuffer, 1);
  
  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();
  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;
}
