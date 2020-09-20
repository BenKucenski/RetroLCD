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
#include "PungLogo.h"
#include "PungWin.h"
#include "PungLose.h"

// graphics stuff
char StringBuffer[32];


#define ROTARY_CLOCK 3
#define ROTARY_DATA 5
#define ROTARY_SWITCH 4

#define GAME_STATE_TITLE 0
#define GAME_STATE_PLAY 1
#define GAME_STATE_WIN 2
#define GAME_STATE_LOSE 3

#define PADDLE_HALF_WIDTH 7

RotaryDial Player1Dial;

int GameState;

float OpponentY;
float BallX, BallY, BallDX, BallDY;
float BallSpeedMax = 0.125;

byte PlayerPoints, OpponentPoints, Level;

byte RenderFrame;

int PlayerPaddleStart, PlayerPaddleEnd;
int OpponentPaddleStart, OpponentPaddleEnd;

int LoopsPerSecond, FramesPerSecond;
int LastLoopsPerSecond, LastFramesPerSecond;

unsigned long LastMillis, FrameMillis, LogicMillis;
unsigned long CurrentMillis;

void setup() {
  Wire.begin();

  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  randomSeed(analogRead(0));

  Player1Dial.Init(ROTARY_CLOCK, ROTARY_DATA, ROTARY_SWITCH);
  attachInterrupt(digitalPinToInterrupt(Player1Dial.ClockPin), ReadRotaryDial, CHANGE);

  GameState = GAME_STATE_TITLE;
}

void StartGame(byte pLevel)
{
  Level = pLevel;
  PlayerPoints = 0;
  OpponentPoints = 0;
  LoopsPerSecond = 0;
  FramesPerSecond = 0;
  LastLoopsPerSecond = 0;
  LastFramesPerSecond = 0;
  LastMillis = millis();
  FrameMillis = millis();
  LogicMillis = millis();

  BallSpeedMax = Level * 0.25;
  RenderFrame = 1;
  ResetBall();

}

void AI()
{
  /* // Perfect Opponent
    OpponentY = BallY - 32;
  */

  /* // Only move when the ball is approaching and on opponents side
    if(BallDX < 0 && BallX < 64) {
    OpponentY = BallY - 32;
    }
  */

  if (BallDX < 0 && BallX < 64) {
    if (OpponentY < BallY - 32) {
      OpponentY += Level * 0.125 / 2.0;
    } else {
      if (OpponentY > BallY - 32) {
        OpponentY -= Level * 0.125 / 2.0;
      }
    }
  }
}

void ResetBall()
{
  BallX = 64;
  BallY = 32;

  BallDY = 0;
  BallDX = random(2) == 0 ? -.1 : .1;

  // center players
  OpponentY = 0;
  Player1Dial.rotaryCounter = 0;
}

void ReadController()
{
  Player1Dial.ReadSwitch();

  if (Player1Dial.rotaryCounter < -16) {
    Player1Dial.rotaryCounter = -16;
  }

  if (Player1Dial.rotaryCounter > 15) {
    Player1Dial.rotaryCounter = 15;
  }

  PlayerPaddleStart = Player1Dial.rotaryCounter * 2 + 32 - PADDLE_HALF_WIDTH;
  PlayerPaddleEnd = Player1Dial.rotaryCounter * 2 + 32 + PADDLE_HALF_WIDTH;

  OpponentPaddleStart = OpponentY + 32 - PADDLE_HALF_WIDTH;
  OpponentPaddleEnd = OpponentY + 32 + PADDLE_HALF_WIDTH;
}

void ReadRotaryDial()
{
  Player1Dial.ReadRotaryDial();
}


void GameLogic()
{
  // ball logic
  BallX += BallDX;
  BallY += BallDY;

  if (BallY <= 0) {
    BallY = 0;
    BallDY = -BallDY;
    RenderFrame = 1;
  }

  if (BallY >= 63) {
    BallY = 63;
    BallDY = -BallDY;
    RenderFrame = 1;
  }

  if (BallX >= 124) {
    if (BallY >= PlayerPaddleStart && BallY <= PlayerPaddleEnd) {
      float Check = ((float)BallY - (float)PlayerPaddleStart) * 0.5 / ((float)PlayerPaddleEnd - (float)PlayerPaddleStart) - 0.25;
      BallDY += Check;
      BallX = 124;
      BallDX = -BallDX;
      RenderFrame = 1;
    } else {
      ResetBall();
      OpponentPoints++;
      if (OpponentPoints == 10) {
        GameState = GAME_STATE_LOSE;
      }
      RenderFrame = 1;
    }
  }

  if (BallX <= 4) {
    if (BallY >= OpponentPaddleStart && BallY <= OpponentPaddleEnd) {
      float Check = ((float)BallY - (float)OpponentPaddleStart) * 0.5 / ((float)OpponentPaddleEnd - (float)OpponentPaddleStart) - 0.25;
      BallDY += Check;

      BallX = 4;
      BallDX = -BallDX;
      RenderFrame = 1;
    } else {
      ResetBall();
      PlayerPoints++;
      if (PlayerPoints == 10) {
        GameState = GAME_STATE_WIN;
      }
      RenderFrame = 1;
    }
  }

  if (BallDY > BallSpeedMax) {
    BallDY = BallSpeedMax;
  }
  if (BallDY < -BallSpeedMax) {
    BallDY = -BallSpeedMax;
  }
}

void RenderDisplay()
{
  Display128x64::ClearDisplay();

  //player
  Display128x64::DrawLine(124, PlayerPaddleStart, 124 , PlayerPaddleEnd, 1);

  //opponent
  Display128x64::DrawLine(4, OpponentPaddleStart, 4, OpponentPaddleEnd, 1);

  // ball
  Display128x64::PlotPixel((int)BallX, (int)BallY, 1);

  //center
  Display128x64::DrawDashedLine(63, 0, 63, 63, 1, 4);
  Display128x64::DrawDashedLine(64, 0, 64, 63, 1, 4);

  // Stats
  sprintf(StringBuffer, "%d %d", LastFramesPerSecond, LastLoopsPerSecond);
  Display128x64::BlitText58(16, 16, LCD1602, StringBuffer);

  // Points
  sprintf(StringBuffer, "%d", OpponentPoints);
  Display128x64::BlitText58(51, 1, LCD1602, StringBuffer);

  sprintf(StringBuffer, "%d", PlayerPoints);
  Display128x64::BlitText58(66, 1, LCD1602, StringBuffer);

  // Level
  sprintf(StringBuffer, "Level: %d", Level);
  Display128x64::BlitText58(50, 54, LCD1602, StringBuffer);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}

void GameLoop()
{
  ReadController();

  CurrentMillis = millis();

  if (CurrentMillis - LogicMillis >= 1) {
    GameLogic();
    AI();
    LoopsPerSecond++;
    LogicMillis = CurrentMillis;
  }


  if (CurrentMillis - FrameMillis >= 75) {
    RenderFrame = 1;
    FrameMillis = CurrentMillis;
  }


  if (RenderFrame) {
    RenderFrame = 0;
    RenderDisplay();
    FramesPerSecond++;
  }


  if (CurrentMillis - LastMillis >= 1000) {
    LastMillis = CurrentMillis;
    LastFramesPerSecond = FramesPerSecond;
    LastLoopsPerSecond = LoopsPerSecond;
    FramesPerSecond = 0;
    LoopsPerSecond = 0;
  }
}

byte _j, _x, _y;
void TitleLoop()
{
  Player1Dial.ReadSwitch();

  Display128x64::ClearDisplay();

  for (_j = 0; _j < 128; _j++)
  {
    _x = (_j % 16) * 8;
    _y = (_j / 16) * 8;
    Display128x64::Blit8(_x, _y, PungLogo + _j * 8);
  }

  if (Player1Dial.rotarySwitchMomentary) {
    StartGame(1);
    GameState = GAME_STATE_PLAY;
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}

void WinLoop()
{
  Player1Dial.ReadSwitch();

  Display128x64::ClearDisplay();

  for (_j = 0; _j < 128; _j++)
  {
    _x = (_j % 16) * 8;
    _y = (_j / 16) * 8;
    Display128x64::Blit8(_x, _y, PungWin + _j * 8);
  }

  if (Player1Dial.rotarySwitchMomentary) {
    Level++;
    StartGame(Level);
    GameState = GAME_STATE_PLAY;
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();

}

void LoseLoop()
{
  Player1Dial.ReadSwitch();

  Display128x64::ClearDisplay();


  for (_j = 0; _j < 128; _j++)
  {
    _x = (_j % 16) * 8;
    _y = (_j / 16) * 8;
    Display128x64::Blit8(_x, _y, PungLose + _j * 8);
  }

  if (Player1Dial.rotarySwitchMomentary) {
    StartGame(1);
    GameState = GAME_STATE_PLAY;
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();

}

void loop() {

  switch (GameState) {
    case GAME_STATE_TITLE: // title screen
      TitleLoop();
      break;
    case GAME_STATE_PLAY: // game
      GameLoop();
      break;
    case GAME_STATE_WIN: // win
      WinLoop();
      break;
    case GAME_STATE_LOSE: // lose
      LoseLoop();
      break;
  }

}
