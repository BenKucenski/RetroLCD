/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include <Arduino.h>
#include <Wire.h>
#include "Display128x64.h"
#include "LCD1602.h"
#include "RotaryDial.h"

// graphics stuff
byte CurrentFrame;
char StringBuffer[32];


#define ROTARY_CLOCK 3
#define ROTARY_DATA 5
#define ROTARY_SWITCH 4

RotaryDial Player1Dial;


void setup() {
  Wire.begin();

  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  CurrentFrame = 0;
  randomSeed(analogRead(0));

  Player1Dial.Init(ROTARY_CLOCK, ROTARY_DATA, ROTARY_SWITCH);
  attachInterrupt(digitalPinToInterrupt(Player1Dial.ClockPin), ReadRotaryDial, CHANGE);
}

void ReadController()
{
  Player1Dial.ReadSwitch();
}

void ReadRotaryDial() 
{
  Player1Dial.ReadRotaryDial();
}



void RenderDisplay()
{
  if (CurrentFrame % 24 > 0) {
    return;
  }

  Display128x64::ClearDisplay();

  sprintf(StringBuffer, "%d %d %d %d %d %d", Player1Dial.rotaryCounter, Player1Dial.rotarySwitch, Player1Dial.rotarySwitchMomentary, Player1Dial.rotaryClock, Player1Dial.rotaryLastClock, Player1Dial.rotaryDirection);
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
