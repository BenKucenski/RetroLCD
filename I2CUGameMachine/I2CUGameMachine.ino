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
#include "LiquidCrystal_I2C.h"
#include "Nunchuk.h"

// graphics stuff
byte CurrentFrame;
char StringBuffer[32];


#define JOYSTICK_XAXIS 1
#define JOYSTICK_YAXIS 2
#define JOYSTICK_BUTTON 8

#define ROTARYA_CLOCK 2
#define ROTARYA_SWITCH 4
#define ROTARYA_DATA 5

#define ROTARYB_CLOCK 3
#define ROTARYB_SWITCH 6
#define ROTARYB_DATA 7

RotaryDial Player1Dial;
RotaryDial Player2Dial;
Joystick PlayerJoystick;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int XPos, YPos;

// Wii Nunchuk
float InputForceX;
float InputForceY;
byte InputC; // can be held down
byte InputZ;
byte InputCAlt; // can't be held down
byte InputZAlt;


void setup() {
  Wire.begin();

  // Init OLED
  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  // Initialize Rotary Dials
  Player1Dial.Init(ROTARYA_CLOCK, ROTARYA_DATA, ROTARYA_SWITCH);
  Player2Dial.Init(ROTARYB_CLOCK, ROTARYB_DATA, ROTARYB_SWITCH);

  attachInterrupt(digitalPinToInterrupt(Player1Dial.ClockPin), ReadRotaryDialA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Player2Dial.ClockPin), ReadRotaryDialB, CHANGE);

  // Initialize Joystick
  PlayerJoystick.Init(JOYSTICK_XAXIS, JOYSTICK_YAXIS, JOYSTICK_BUTTON);
  

  // initialize the LCD Character Display
  lcd.begin();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);

  lcd.print("Hello World");

  CurrentFrame = 0;
  randomSeed(analogRead(0));
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
  Display128x64::BlitText58N(0, 0, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "%d %d %d %d %d %d", Player2Dial.rotaryCounter, Player2Dial.rotarySwitch, Player2Dial.rotarySwitchMomentary, Player2Dial.rotaryClock, Player2Dial.rotaryLastClock, Player2Dial.rotaryDirection);
  Display128x64::BlitText58N(0, 9, LCD1602, StringBuffer, 1);


  sprintf(StringBuffer, "%d %d %d %d", PlayerJoystick.XAxis, PlayerJoystick.YAxis, PlayerJoystick.joySwitch, PlayerJoystick.joySwitchMomentary);
  Display128x64::BlitText58N(0, 18, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "%d %d", XPos, YPos);
  Display128x64::BlitText58N(0, 27, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "%d %d %d %d %d %d", (int)(InputForceX * 128), (int)(InputForceY * 128), InputC, InputZ, InputCAlt, InputZAlt);
  Display128x64::BlitText58N(0, 36, LCD1602, StringBuffer, 1);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();
  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;
}
