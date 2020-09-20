/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/

#define CLOCK_DISPLAY 100000
 
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

float XPos;
float YPos;
byte Color;

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

  XPos = 64;
  YPos = 32;
  Color = 1;
  Display128x64::PlotPixel(XPos, YPos, Color);
}

void ReadController()
{
  Player1Dial.ReadSwitch();
  Player2Dial.ReadSwitch();
}

void ReadRotaryDialA()
{
  Player1Dial.ReadRotaryDial();
  if (Player1Dial.rotaryDirection == 1) {
    YPos -= 0.25;
  } else {
    YPos += 0.25;
  }
  Display128x64::PlotPixel(XPos, YPos, Color);
}

void ReadRotaryDialB()
{
  Player2Dial.ReadRotaryDial();
  if (Player2Dial.rotaryDirection == 1) {
    XPos += 0.25;
  } else {
    XPos -= 0.25;
  }
  Display128x64::PlotPixel(XPos, YPos, Color);
}


void RenderDisplay()
{

  if (Player1Dial.rotarySwitch) {
    Display128x64::ClearDisplay();
    Color = 1;
    Display128x64::PlotPixel(XPos, YPos, Color);
  }

  if (Player2Dial.rotarySwitch == 1) {
    Color = 0;
  } else {
    Color = 1;
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();

  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;
  delay(100);
}
