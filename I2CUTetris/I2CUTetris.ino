/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include <Wire.h>

#include "Arduino.h"
#include "Tetris.h"
#include "nunchuk.h"
#include "LCD1602.h"
#include "Display128x64.h"


Tetris Tetris;

int x, y, frame;
unsigned long time;
char StringBuffer[32];

int PlayerX, PlayerY, PlayerPieceNumber, PlayerScore;
TetrisPiece PlayerPiece;

void setup() {
  Wire.begin();
  Nunchuk::NunchukInit();
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  Nunchuk::NunchukRead();
  delay(100);
  randomSeed(analogRead(0));


  Tetris.Init(8, 16);

  frame = 0;
  Tetris.ClearScreen();

  time = millis();

  PlayerPieceNumber = -1;
  PlayerScore = 0;
}

void HandleInput()
{
  Wire.setClock(CLOCK_NUNCHUK);

  if (!Nunchuk::NunchukRead()) {
    return;
  }

  float InputForceX;
  float InputForceY;

  InputForceX = -(map(Nunchuk::NunchukX(), -128, 127, 0, 255) * 1.0 / 128.0 - 1.0);

  if (InputForceX < 0) {
    PlayerX = Tetris.MoveLeft(PlayerX, PlayerY, PlayerPiece);
  }

  if (InputForceX > 0) {
    PlayerX = Tetris.MoveRight(PlayerX, PlayerY, PlayerPiece);
  }

  if (Nunchuk::NunchukCAlt()) {
    PlayerPiece = Tetris.RotateLeft(PlayerX, PlayerY, PlayerPiece);
  }

  if (Nunchuk::NunchukZAlt()) {
    PlayerPiece = Tetris.RotateRight(PlayerX, PlayerY, PlayerPiece);
  }
}


void RenderDisplay() {
  char line[17];
  byte point_char[16];
  int i, j;

  Display128x64::ClearDisplay();

  if (PlayerScore < 0) {
    PlayerScore = 0;
  }
 
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 16; j++) {
      if (Tetris.playfield[i + j * 8] == 1) {
        Display128x64::Blit8((15 - j) * 8, (7 - i) * 8, SPRITE_TETRIS_BLOCK);
      }
    }
  }


  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (Tetris.GetPiecePixel(i, j, PlayerPiece)) {
        Display128x64::Blit8((15 - (PlayerY + j)) * 8, (7 - (PlayerX + i)) * 8, SPRITE_TETRIS_BLOCK);
      }
    }
  }

  sprintf (StringBuffer, "Pts: %d", PlayerScore);
  Display128x64::BlitText58(84, 56, LCD1602, StringBuffer);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}

void loop() {
  if (PlayerPieceNumber < 0) {
    PlayerPieceNumber = rand() % 7;
    PlayerY = -3;
    PlayerX = (8 - 4) / 2; // (board width - piece width) divided by 2
    PlayerPiece = Tetris.BlockSprites[PlayerPieceNumber];

    for (int i = 0; i < 4; i++) {
      if (Tetris.CheckScreenCollision(PlayerX, PlayerY + i,
                                      PlayerPiece)) {
        Tetris.ClearScreen();
        PlayerScore -= 10;
        PlayerPieceNumber = -1;
      }
    }
  }

  if (millis() - time > 500) {
    if (Tetris.CheckBottomCollision(PlayerX, PlayerY, PlayerPiece)) {
      Tetris.StickPiece(PlayerX, PlayerY, PlayerPiece);
      PlayerScore += Tetris.CheckCompletedRows();
      PlayerPieceNumber = -1;
    } else {
      PlayerY++;
    }
    time = millis();
  }

  HandleInput();

  RenderDisplay();
  delay(50);
}
