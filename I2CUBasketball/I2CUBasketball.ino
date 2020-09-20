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

#include "Basketball.h"

byte CurrentFrame;
char StringBuffer[32];

Basketball Game;

void setup() {
  Wire.begin();

  Nunchuk::NunchukInit();
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  Nunchuk::NunchukRead();
  delay(100);
  randomSeed(analogRead(0));

  Game.ResetGame();
  CurrentFrame = 0;
}

void RenderDisplay()
{
  Display128x64::ClearDisplay();

  byte PlayerX = Game.GetPlayerX(Game.PlayerPosition);
  byte PlayerY = Game.GetPlayerY(Game.PlayerPosition);
  byte x, y;

  for (x  = 0; x < 5; x++) {
    for (y  = 0; y < 4; y++) {
      if (Game.GetPixel(x, y)) {
        if (x == PlayerX && y == PlayerY) {
          Display128x64::Blit8(x * 24, 8 * (4 - y), BASKETBALL_SPRITES + 24); // 24 - defender

        } else {
          Display128x64::Blit8(x * 24, 8 * (4 - y), BASKETBALL_SPRITES + 16); // 16 - player
        }
      }
    }
  }

  // basket
  if (Game.Display[2] & 16) {
    Display128x64::BlitMask8(48, 0, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 22 - ball
  } else {
    Display128x64::Blit8(48, 0, BASKETBALL_SPRITES + 32); // 22 - basket
  }

  // basketball
  if (Game.Display[2] & 32) {
    Display128x64::BlitMask8(0, 8 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 22
  }

  if (Game.Display[2] & 64) {
    Display128x64::BlitMask8(24, 8 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 23
  }
  if (Game.Display[2] & 128) {
    Display128x64::BlitMask8(0, 24 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 24
  }
  if (Game.Display[3] & 1) {
    Display128x64::BlitMask8(24, 16 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 25
  }
  if (Game.Display[3] & 2) {
    Display128x64::BlitMask8(48, 32 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 26
  }
  if (Game.Display[3] & 4) {
    Display128x64::BlitMask8(48, 24 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 27
  }
  if (Game.Display[3] & 8) {
    Display128x64::BlitMask8(48, 16 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 28
  }
  if (Game.Display[3] & 16) {
    Display128x64::BlitMask8(96, 24 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 29
  }
  if (Game.Display[3] & 32) {
    Display128x64::BlitMask8(72, 16 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 30
  }
  if (Game.Display[3] & 64) {
    Display128x64::BlitMask8(96, 8 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 31
  }
  if (Game.Display[3] & 128) {
    Display128x64::BlitMask8(72, 8 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 32
  }
  if (Game.Display[4] & 1) {
    Display128x64::BlitMask8(48, 8 + 2, BASKETBALL_SPRITES, BASKETBALL_SPRITES + 8); // 33
  }

  // indicators
  Display128x64::PlotPixel(0, 48, Game.Display[4] & 2 ? 1 : 0); // 34
  Display128x64::PlotPixel(16, 48, Game.Display[4] & 4 ? 1 : 0); // 35
  Display128x64::PlotPixel(32, 48, Game.Display[4] & 8 ? 1 : 0); // 36
  Display128x64::PlotPixel(48, 48, Game.Display[4] & 16 ? 1 : 0); // 37
  Display128x64::PlotPixel(64, 48, Game.Display[4] & 32 ? 1 : 0); // 38
  Display128x64::PlotPixel(80, 48, Game.Display[4] & 64 ? 1 : 0); // 39
  Display128x64::PlotPixel(96, 48, Game.Display[4] & 128 ? 1 : 0); // 40


  sprintf(StringBuffer, "Quarter: %d - %d:%02d", Game.CurrentQuarter, Game.CurrentQuarterMinutes, Game.CurrentQuarterSeconds);
  Display128x64::BlitText58(0, 48, LCD1602, StringBuffer);


  sprintf(StringBuffer, "Home: %d Visitor: %d", Game.ScoreHome, Game.ScoreVisitor);
  Display128x64::BlitText58(0, 56, LCD1602, StringBuffer);


  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}



void ReadController()
{
  Wire.setClock(CLOCK_NUNCHUK);

  if (!Nunchuk::NunchukRead()) {
    return;
  }

  float InputForceX;
  float InputForceY;

  InputForceX = map(Nunchuk::NunchukX(), -128, 127, 0, 255) * 1.0 / 128.0 - 1.0;
  InputForceY = (255 - map(Nunchuk::NunchukY(), 127, -128, 0, 255)) * 1.0 / 128.0 - 1.0;

  if (Nunchuk::NunchukCAlt()) {
    // Shot = NULL; // uncomment this for testing
    if (Game.Shot == NULL) {
      Game.StartShot(Game.PlayerPosition);
    }
  }

  if (InputForceY > 0) {
    Game.MovePlayer(CONTROLLER_BUTTON_UP);
  }

  if (InputForceX < 0) {
    Game.MovePlayer(CONTROLLER_BUTTON_LEFT);
  }

  if (InputForceY < 0) {
    Game.MovePlayer(CONTROLLER_BUTTON_DOWN);
  }

  if (InputForceX > 0) {
    Game.MovePlayer(CONTROLLER_BUTTON_RIGHT);
  }
}

void loop() {
  ReadController();

  Game.Update(CurrentFrame);

  Game.RenderCourt();
  RenderDisplay();


  CurrentFrame++;
  CurrentFrame %= 240;

  Game.HandleShot(CurrentFrame);


  delay(50); // each frame is 50 milliseconds
}
