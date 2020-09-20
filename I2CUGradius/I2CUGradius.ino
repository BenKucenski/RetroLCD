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
#include "Gradius.h"

#define PLAYER_VELOCITY 3

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


// player position
int PlayerX, PlayerY;
char PlayerXOff, PlayerYOff;

// screen position
int ScreenX, ScreenY;
char ScreenXOff, ScreenYOff;


void setup() {
  Wire.begin();

  Nunchuk::NunchukInit();
  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  Nunchuk::NunchukRead();
  delay(100);
  randomSeed(analogRead(0));

  CurrentFrame = 0;
  PlayerX = 200;
  PlayerY = 12;
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

  if(InputForceX > 0.5) {
    PlayerXOff += PLAYER_VELOCITY;
  }
  if(InputForceX < -0.5) {
    PlayerXOff -= PLAYER_VELOCITY;
  }

  if(InputForceY > 0.5) {
    PlayerYOff -= PLAYER_VELOCITY;
  }
  if(InputForceY < -0.5) {
    PlayerYOff += PLAYER_VELOCITY;
  }

  if(PlayerXOff >= 8) {
    PlayerX += PlayerXOff / 8;
    PlayerXOff %= 8;
  }

  if(PlayerXOff < 0) {
    PlayerX --;
    PlayerXOff = 8 + PlayerXOff;
  }

  if(PlayerYOff >= 8) {
    PlayerY += PlayerYOff / 8;
    PlayerYOff %= 8;
  }

  if(PlayerYOff < 0) {
    PlayerY --;
    PlayerYOff = 8 + PlayerYOff;
  }
}

char sx, sy;
byte x, y, b;

void RenderDisplay()
{
  Display128x64::ClearDisplay();

  ScreenY = PlayerY - 4;
  ScreenX = PlayerX - 8;
  ScreenXOff = PlayerXOff;
  ScreenYOff = PlayerYOff;
  
  if (PlayerX < 8) {
    ScreenX = 0;
    ScreenXOff = 0;
  }

  if (PlayerY < 4) {
    ScreenY = 0;
    ScreenYOff = 0;
  }
  
  if (PlayerX >= GRADIUS_LEVEL1_WIDTH - 16) {
    ScreenX = GRADIUS_LEVEL1_WIDTH - 16 - 8;
    ScreenXOff = 0;
  }

  if (PlayerY >= GRADIUS_LEVEL1_HEIGHT - 4) {
    ScreenY = GRADIUS_LEVEL1_HEIGHT - 4 - 4;
    ScreenYOff = 0;
  }

  sx = 0;
  sy = 0;
  for (x = ScreenX; x < ScreenX - 1 + 18; x++) {
    sy = 0;
    for (y = ScreenY; y < ScreenY + 9; y++) {
      if(y * GRADIUS_LEVEL1_WIDTH + x >= GRADIUS_LEVEL1_WIDTH * GRADIUS_LEVEL1_HEIGHT) {
        continue;
      }
      b = pgm_read_byte(GRADIUS_LEVEL1_MAP + y * GRADIUS_LEVEL1_WIDTH + x);
      Display128x64::Blit8Fast(sx * 8 - ScreenXOff, sy * 8 - ScreenYOff, GRADIUS_LEVEL1_TILES + b * 8);
      sy++;
    }
    sx++;
  }

  sx = (PlayerX - ScreenX) * 8 + (ScreenXOff == 0 ? PlayerXOff : 0);
  sy = (PlayerY - ScreenY) * 8 + (ScreenYOff == 0 ? PlayerYOff : 0);
  sprintf(StringBuffer, "%s","PLAY");
  Display128x64::BlitText58N(sx, sy, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "%d %d %d %d",PlayerX, PlayerY, PlayerXOff, PlayerYOff);
  Display128x64::BlitText58N(0, 56, LCD1602, StringBuffer, 1);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();
  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;
}
