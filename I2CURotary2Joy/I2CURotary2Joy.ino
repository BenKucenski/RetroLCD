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
#include "Joystick.h"

// graphics stuff
byte CurrentFrame;
char StringBuffer[32];


#define JOYSTICK_XAXIS 1
#define JOYSTICK_YAXIS 2
#define JOYSTICK_BUTTON 6

#define ROTARYA_CLOCK 3
#define ROTARYA_DATA 5
#define ROTARYA_SWITCH 4

#define ROTARYB_CLOCK 2
#define ROTARYB_DATA 9
#define ROTARYB_SWITCH 8

RotaryDial Player1Dial;
RotaryDial Player2Dial;
Joystick PlayerJoystick;

int XPos, YPos;


void setup() {
  Wire.begin();

  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  CurrentFrame = 0;
  randomSeed(analogRead(0));

  Player1Dial.Init(ROTARYA_CLOCK, ROTARYA_DATA, ROTARYA_SWITCH);
  Player2Dial.Init(ROTARYB_CLOCK, ROTARYB_DATA, ROTARYB_SWITCH);

  PlayerJoystick.Init(JOYSTICK_XAXIS, JOYSTICK_YAXIS, JOYSTICK_BUTTON);
  
  attachInterrupt(digitalPinToInterrupt(Player1Dial.ClockPin), ReadRotaryDialA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Player2Dial.ClockPin), ReadRotaryDialB, CHANGE);
}

void ReadController()
{
  Player1Dial.ReadSwitch();
  Player2Dial.ReadSwitch();

  PlayerJoystick.ReadSwitch();
  PlayerJoystick.ReadJoystick();

  // Y - 0 = Up
  // Y ~350 - 370 - Center
  // Y - 700 = Down
  
  // X - 0 = Right
  // X ~350 - 370 - Center
  // X - 700 = Left

  XPos = PlayerJoystick.XAxis;
  if(XPos > 700) {
    XPos = 700;
  }
  if(XPos >= 340 && XPos <= 360) {
    XPos = 350;
  }
  XPos = (700 - XPos) - 350;
  XPos = (float)(XPos) * 63.5 / 350.0 + 64.0;
  

  YPos = PlayerJoystick.YAxis;
  if(YPos > 700) {
    YPos = 700;
  }
  if(YPos >= 340 && YPos <= 360) {
    YPos = 350;
  }
  YPos = YPos - 350;
  YPos = (float)(YPos) * 31.5 / 350.0 + 32.0;
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


  sprintf(StringBuffer, "%d %d %d %d", PlayerJoystick.XAxis, PlayerJoystick.YAxis, PlayerJoystick.joySwitch, PlayerJoystick.joySwitchMomentary);
  Display128x64::BlitText58N(0, 37, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "%d %d", XPos, YPos);
  Display128x64::BlitText58N(0, 53, LCD1602, StringBuffer, 1);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();
  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;
}