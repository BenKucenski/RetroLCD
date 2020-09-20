/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include <Arduino.h>
#include "Basketball.h"

void Basketball::ClearScreen()
{
  Display[0] = 0;
  Display[1] = 0;
  Display[2] = 0;
  Display[3] = 0;
  Display[4] = 0;
}

void Basketball::PlotPixel(byte x, byte y, byte c)
{
  byte b = (x / 2) % 3;
  byte i = ((x % 2) * 4 + y) % 8;

  if (c) {
    Display[b] |= 1 << i;
  } else {
    Display[b] &= ~(1 << i);
  }
}

byte Basketball::GetPixel(byte x, byte y)
{
  byte b = (x / 2) % 3;
  byte i = ((x % 2) * 4 + y) % 8;

  return (Display[b] >> i) & 1;
}

byte Basketball::OpenPosition(byte x, byte y)
{
  byte p;
  SetPlayerPosition(x, y, p);
  if (p == PlayerPosition) {
    return false;
  }
  if (p == EnemyPosition[0]) {
    return false;
  }
  if (p == EnemyPosition[1]) {
    return false;
  }
  if (p == EnemyPosition[2]) {
    return false;
  }
  if (p == EnemyPosition[3]) {
    return false;
  }
  if (p == EnemyPosition[4]) {
    return false;
  }
}

byte Basketball::CheckPosition(byte x, byte y)
{
  byte o = 0;
  byte q = 0;
  byte px = (PlayerPosition >> 4) % 16;
  byte py = PlayerPosition % 16;

  if (x > 0 && OpenPosition(x - 1, y)) {
    o |= CONTROLLER_BUTTON_LEFT;
  }

  if (x < 4 && OpenPosition(x + 1, y)) {
    o |= CONTROLLER_BUTTON_RIGHT;
  }

  if (y > 0 && OpenPosition(x, y - 1)) {
    o |= CONTROLLER_BUTTON_DOWN;
  }

  if (y < 3 && OpenPosition(x, y + 1)) {
    o |= CONTROLLER_BUTTON_UP;
  }
  return o;
}

void Basketball::MoveEnemies()
{
  byte px = (PlayerPosition >> 4) % 16;
  byte py = PlayerPosition % 16;

  byte ex;
  byte ey;
  byte o;
  byte moved = 0;
  byte attempts = 0;
  while (moved < 1 && attempts < 5) {
    attempts++;
    last_moved++;
    last_moved %= 5;
    ex = (EnemyPosition[last_moved] >> 4) % 16;
    ey = EnemyPosition[last_moved] % 16;
    o = CheckPosition(ex, ey);
    if (ex < px && (o & CONTROLLER_BUTTON_RIGHT)) {
      MovePlayerPosition(1, 0, EnemyPosition[last_moved]);
      last_moved++;
      moved++;
      continue;
    }

    if (ex > px && (o & CONTROLLER_BUTTON_LEFT)) {
      MovePlayerPosition(15, 0, EnemyPosition[last_moved]);
      moved++;
      continue;
    }

    if (ey > 1 && ey > py && (o & CONTROLLER_BUTTON_DOWN)) {
      MovePlayerPosition(0, 15, EnemyPosition[last_moved]);
      moved++;
      continue;
    }

    if (ey < py && (o & CONTROLLER_BUTTON_UP)) {
      MovePlayerPosition(0, 1, EnemyPosition[last_moved]);
      moved++;
      continue;
    }
  }
}

void Basketball::CheckBall()
{
  if (Shot == NULL) {
    return;
  }

  byte ball = BallConversion[Shot[ShotStep] - 22];
  for (byte i = 0; i < 5; i++) {
    if (ball == EnemyPosition[i]) {
      Shot = NULL;
      SwitchSides(0);
      return;
    }
  }
}

byte Basketball::MovePlayer(byte dir)
{
  byte last_p = PlayerPosition;
  byte px = (PlayerPosition >> 4) % 16;
  byte py = PlayerPosition % 16;

  byte PlayerMoved = 0;
  if (dir == CONTROLLER_BUTTON_UP) {
    if (py == 3) {
      return false;
    } else {
      MovePlayerPosition(0, 1, PlayerPosition);
      PlayerMoved = 1;
    }
  }

  if (dir == CONTROLLER_BUTTON_DOWN) {
    if (py == 0) {
      return false;
    } else {
      MovePlayerPosition(0, 15, PlayerPosition);
      PlayerMoved = 1;
    }
  }

  if (dir == CONTROLLER_BUTTON_LEFT) {
    if (px == 0) {
      return false;
    } else {
      MovePlayerPosition(15, 0, PlayerPosition);
      PlayerMoved = 1;
    }
  }

  if (dir == CONTROLLER_BUTTON_RIGHT) {
    if (px == 4) {
      return false;
    } else {
      MovePlayerPosition(1, 0, PlayerPosition);
      PlayerMoved = 1;
    }
  }

  if (PlayerPosition == EnemyPosition[0]) {
    PlayerPosition = last_p;
    return false;
  }
  if (PlayerPosition == EnemyPosition[1]) {
    PlayerPosition = last_p;
    return false;
  }
  if (PlayerPosition == EnemyPosition[2]) {
    PlayerPosition = last_p;
    return false;
  }
  if (PlayerPosition == EnemyPosition[3]) {
    PlayerPosition = last_p;
    return false;
  }
  if (PlayerPosition == EnemyPosition[4]) {
    PlayerPosition = last_p;
    return false;
  }

  if (PlayerMoved && ShotTimer == 0) {
    // 96 game seconds on clock when shot clock is 24 seconds
    if (CurrentQuarterMinutes <= 1 && CurrentQuarterMinutes * 60 + CurrentQuarterSeconds < 96) {
      ShotTimer = (CurrentQuarterMinutes * 60 + CurrentQuarterSeconds) / 4;
    } else {
      ShotTimer = 24;
    }
  }

  return true;
}

void Basketball::MovePlayerPosition(byte x, byte y, byte & p)
{
  byte px = (p >> 4) % 16;
  byte py = p % 16;

  px += x;
  px %= 16;

  py += y;
  py %= 16;

  p = ((px << 4) & 0b11110000) | (py & 0b00001111);
}

byte Basketball::GetPlayerX(byte & p)
{
  return (p >> 4) % 16;
}

byte Basketball::GetPlayerY(byte & p)
{
  return p % 16;
}

void Basketball::SetPlayerPosition(byte x, byte y, byte & p)
{
  p = (x << 4) & 0b11110000 | (y & 0b00001111);
}


/////////////////////


void Basketball::RenderCourt()
{
  ClearScreen();

  byte px = PlayerPosition >> 4;
  byte py = PlayerPosition % 16;
  PlotPixel(px, py, 1);

  for (byte j = 0; j < 5; j++) {
    px = EnemyPosition[j] >> 4;
    py = EnemyPosition[j] % 16;
    PlotPixel(px, py, 1);
  }

  if (Shot != NULL) {
    byte ball = Shot[ShotStep] - 1;
    byte b = ball / 8;
    byte i = ball % 8;
    Display[b] |= 1 << i;
  }
}


void Basketball::ResetEnemyPositions()
{
  SetPlayerPosition(0, 3, EnemyPosition[0]);
  SetPlayerPosition(1, 2, EnemyPosition[1]);
  SetPlayerPosition(2, 2, EnemyPosition[2]);
  SetPlayerPosition(3, 2, EnemyPosition[3]);
  SetPlayerPosition(4, 3, EnemyPosition[4]);
}

void Basketball::ResetGame()
{
  CurrentQuarterMinutes = QUARTER_MINUTES - 1;
  CurrentQuarter = 1;
  CurrentQuarterSeconds = 60;
  SetPlayerPosition(0, 0, PlayerPosition);
  CurrentCounter = 0;
  Shot = 0;
  ShotStep = 0;
  ScoreMode = SCORE_MODE_HOME;
  ShotTimer = 0;
  ResetEnemyPositions();
}

void Basketball::StartShot(byte & p)
{
  if (Shot != NULL) {
    return;
  }

  byte x = GetPlayerX(p);
  byte y = GetPlayerY(p);

  switch (y) {
    case 3:
      if (x <= 2) {
        ShotStep = x;
        Shot = &Shot4[ShotStep];
      } else {
        ShotStep = 4 - x;
        Shot = &Shot20[ShotStep];
      }
      break;
    case 2:
      if (x == 1) {
        ShotStep = 1;
        Shot = &Shot2[ShotStep];
      }
      if (x == 2) {
        ShotStep = 2;
        Shot = &Shot9[ShotStep];
      }
      if (x == 3) {
        ShotStep = 1;
        Shot = &Shot18[ShotStep];
      }
      break;
    case 1:
      if (x == 0) {
        ShotStep = 0;
        Shot = &Shot2[ShotStep];
      }
      if (x == 2) {
        ShotStep = 1;
        Shot = &Shot9[ShotStep];
      }
      if (x == 4) {
        ShotStep = 0;
        Shot = &Shot18[ShotStep];
      }
      break;
    case 0:
      if (x == 2) {
        ShotStep = 0;
        Shot = &Shot9[ShotStep];
      }
      break;
  }
  ShotStep = 0; // reset to zero because now we're pointed directly at the current position within the array
}

void Basketball::SwitchSides(byte ShotPoints)
{
  if (ScoreMode == SCORE_MODE_HOME) {
    ScoreHome += ShotPoints; // home scores
    SetPlayerPosition(4, 0, PlayerPosition); // reset player to visitor position
  } else {
    ScoreVisitor += ShotPoints; // visitor scores
    SetPlayerPosition(0, 0, PlayerPosition); // reset player to home position
  }
  ScoreMode = 1 - ScoreMode;
  ShotTimer = 0;
  ResetEnemyPositions();
}

void Basketball::Update(byte CurrentFrame)
{
  // every 250 milliseconds, count down 1 second of game time
  if (CurrentFrame % 5 == 0) {
    CurrentQuarterSeconds--;

    if (CurrentQuarterMinutes == 0 && CurrentQuarterSeconds == 0) {
      CurrentQuarterMinutes = QUARTER_MINUTES - 1;
      CurrentQuarterSeconds = 60;
      CurrentQuarter++;
      if (CurrentQuarter > 4) {
        ResetGame();
        return;
      }
    }

    if (CurrentQuarterSeconds == 0) {
      CurrentQuarterSeconds = 60;
      CurrentQuarterMinutes--;
    }

  }

  if (CurrentFrame % 30 == 0) { // move enemies once per 1.5 seconds
    MoveEnemies();
  }
}

void Basketball::HandleShot(byte CurrentFrame)
{
  if (Shot != NULL) { // if we're shotting a basket
    if (CurrentFrame % 5 == 0) {
      if (Shot[ShotStep] != 21) {
        ShotStep++;
        if (Shot[ShotStep] == 21) {
          ShotFrameCount = 10;
          byte ShotPoints = ShotStep >= 3 ? 3 : 2;
          SwitchSides(ShotPoints);
        } else {
          CheckBall();
        }
      }
    }
    if (ShotFrameCount) {
      ShotFrameCount--;
      if (ShotFrameCount == 0) {
        Shot = NULL;
      }
    }
  }

  // 20 frames at 50ms each is 1 second
  if (CurrentFrame % 20 == 0) {
    CurrentCounter++;
    CurrentCounter %= 2;

    // shot timer is real time with 24 seconds
    // if 24 seconds expire then if the ball is moving, it resets
    // and the ball changes sides
    if (ShotTimer > 0) {
      ShotTimer--;
      if (ShotTimer == 0) {
        SwitchSides(0);
      }
    }
  }
}
