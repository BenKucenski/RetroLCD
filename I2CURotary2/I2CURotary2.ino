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


#define ROTARYA_CLOCK 3
#define ROTARYA_DATA 5
#define ROTARYA_SWITCH 4

#define ROTARYB_CLOCK 2
#define ROTARYB_DATA 9
#define ROTARYB_SWITCH 8

RotaryDial Player1Dial;
RotaryDial Player2Dial;


void setup() {
  Wire.begin();

  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  CurrentFrame = 0;
  randomSeed(analogRead(0));

  Player1Dial.Init(ROTARYA_CLOCK, ROTARYA_DATA, ROTARYA_SWITCH);
  Player2Dial.Init(ROTARYB_CLOCK, ROTARYB_DATA, ROTARYB_SWITCH);
  attachInterrupt(digitalPinToInterrupt(Player1Dial.ClockPin), ReadRotaryDialA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Player2Dial.ClockPin), ReadRotaryDialB, CHANGE);
}

void ReadController()
{
  Player1Dial.ReadSwitch();
  Player2Dial.ReadSwitch();
}

void ReadRotaryDialA() 
{
  Player1Dial.ReadRotaryDial();
}

void ReadRotaryDialB() 
{
  Player2Dial.ReadRotaryDial();
}


void RenderDisplay()
{
  if (CurrentFrame % 24 > 0) {
    return;
  }

  Display128x64::ClearDisplay();

  sprintf(StringBuffer, "%d %d %d %d %d %d", Player1Dial.rotaryCounter, Player1Dial.rotarySwitch, Player1Dial.rotarySwitchMomentary, Player1Dial.rotaryClock, Player1Dial.rotaryLastClock, Player1Dial.rotaryDirection);
  Display128x64::BlitText58N(0, 5, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "%d %d %d %d %d %d", Player2Dial.rotaryCounter, Player2Dial.rotarySwitch, Player2Dial.rotarySwitchMomentary, Player2Dial.rotaryClock, Player2Dial.rotaryLastClock, Player2Dial.rotaryDirection);
  Display128x64::BlitText58N(0, 21, LCD1602, StringBuffer, 1);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();
  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;
}
