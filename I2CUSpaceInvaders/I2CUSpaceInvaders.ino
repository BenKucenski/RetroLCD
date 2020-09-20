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
#include "Space_Invaders.h"
#include "Space_Invaders_Sprites.h"
#include "Space_Invaders_Title.h"

// based on https://github.com/ccostin93/Space-Invader-Arduino

// graphics stuff
byte CurrentFrame;

// input stuff
float InputForceX;
float InputForceY;
byte InputC; // can be held down
byte InputZ;
byte InputCAlt; // can't be held down
byte InputZAlt;

// game stuff

#define nr_enemy 12
#define chance_shoot nr_enemy * 5 // each enemy has a 1 in 5 chance of shooting every frame

#define GAME_STATE_PLAY 0
#define GAME_STATE_PAUSE 1
#define GAME_STATE_DEFEAT 2
#define GAME_STATE_TITLE 3

#define DEFAULT_PLAYER_X 28

char StringBuffer[32];
char bounce; //the enemy bounces off the wall
char GAME_STATE = 0; // 0 - play, 1 - pause, 2 - defeat

Enemy enemy[nr_enemy];
Bullet ShipBullet;
Bullet EnemyBullet;
Ship ship;

long score = 0;
byte level = 1;

byte remaining_enemies;

byte _i, _j, _x, _y; // generics
byte checkx, checky;


struct Shield {
  byte x, y;
  byte sprite[16];
};

Shield Shields[3];

void addBullet(byte x, byte y, byte newSpeed, char side) {
  //side == 1 -> my ship, else enemy ship
  if (side == 1 && ShipBullet.getExist() == 0) {
    ShipBullet.Init(x, y, newSpeed);
  }
  else if (side == 0 && EnemyBullet.getExist() == 0) {
    EnemyBullet.Init(x, y, newSpeed);
  }
}

void setup() {
  Wire.begin();

  Nunchuk::NunchukInit();
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  Nunchuk::NunchukRead();
  delay(100);
  randomSeed(analogRead(0));

  CurrentFrame = 0;
  level = 1;
  InitLevel();
  GAME_STATE = GAME_STATE_TITLE;
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
}

void InitShields()
{
  for (byte s = 0; s < 3; s++) {
    Shields[s].x = 4 + s * 20;
    Shields[s].y = 112;
    for ( byte b = 0 ; b < 16; b++) {
      Shields[s].sprite[b] = pgm_read_byte(SPACE_INVADERS_SHIELD + b);
    }
  }
}


byte GetShieldPixel(byte x, byte y, byte s, byte offset = 0)
{
  if (s > 2) {
    return 0;
  }

  if (offset == 1) {
    x -= Shields[_i].x;
    y -= Shields[_i].y;
  }

  if (x > 15 || y > 7) {
    return 0;
  }

  byte b = Shields[s].sprite[y + (x / 8) * 8];
  byte p = (b >> (7 - (x % 8))) & 1;
  return p;

}

void SetShieldPixel(byte x, byte y, byte s, byte c, byte offset = 0)
{
  if (s > 2) {
    return;
  }

  if (offset == 1) {
    x -= Shields[_i].x;
    y -= Shields[_i].y;
  }

  if (x > 15 || y > 7) {
    return;
  }

  if (c) {
    Shields[s].sprite[y + (x / 8) * 8]  |= (1 << ((7 - (x % 8)) & 7));
  } else {
    Shields[s].sprite[y + (x / 8) * 8] &= ~(1 << ((7 - (x % 8)) & 7));
  }
}


void InitLevel()
{
  if (level == 1) {
    ship.Init(DEFAULT_PLAYER_X, 120);
    score = 0;
  }
  else {
    ship.setX(24);
    ship.setY(120); //reset ship position
  }

  //Enemy initialization
  for (_i = 0; _i < 4; _i++) {
    for (_j = 0; _j < 3; _j++) {
      enemy[_i * 3 + _j].Init(10 * _i + 2, 9 * _j + 2, level);
    }
  }
  remaining_enemies = 12;
  InitShields();
}

void RenderPlay()
{
  // move player
  if (InputForceX < -0.125 || InputForceX > 0.125) {
    ship.move(InputForceX < 0 ? -1 : 1);
  }

  if (InputCAlt == 1 || InputZAlt == 1) {
    if (ShipBullet.getExist() == 0) {
      ShipBullet.Init(ship.getX() + 4, ship.getY() + 8, -5);
    }
  }

  // move bullets
  if (ShipBullet.getExist() == 1) {//the bullet exists
    ShipBullet.move();
    Display128x64::DrawLine(ShipBullet.getX(), ShipBullet.getY(), ShipBullet.getX(), ShipBullet.getY() - 5, 1);
    for (_i = 0; _i < nr_enemy; _i++) {
      if (ShipBullet.collisionEnemy(enemy[_i]) == 1) {
        enemy[_i].setDead();
        score += 100;
        remaining_enemies--;
        if (remaining_enemies == 0) {
          level++;
          InitLevel();
          return;
        }
        break;
      }
    }

    checkx = ShipBullet.getX();
    checky = ShipBullet.getY();
    for (_i = 0; _i < 3; _i++) {
      if (checkx >= Shields[_i].x && checkx <= Shields[_i].x + 15) {
        for (_j = 0; _j <= 5; _j++) {
          if (checky - _j >= Shields[_i].y && checky - _j <= Shields[_i].y + 7) {
            if (GetShieldPixel(checkx, checky - _j, _i, 1) == 1) {
              SetShieldPixel(checkx, checky - _j, _i, 0, 1);
              SetShieldPixel(checkx - 1, checky - _j, _i, 0, 1);
              SetShieldPixel(checkx + 1, checky - _j, _i, 0, 1);
              SetShieldPixel(checkx, checky - _j - 1, _i, 0, 1);

              ShipBullet.exist = 0;
              break;
            }
          }
        }
      }
    }
  }

  if (EnemyBullet.getExist() == 1) {//the bullet exists
    EnemyBullet.move();
    if (EnemyBullet.collisionShip(ship) == 1) {
      ship.setLives(ship.getLives() - 1);
      ship.setX(DEFAULT_PLAYER_X);
      ship.setY(120); //reset ship position
      if (ship.getLives() == 0) {
        GAME_STATE = GAME_STATE_DEFEAT;
      }
    }

    Display128x64::DrawLine(EnemyBullet.getX(), EnemyBullet.getY(), EnemyBullet.getX(), EnemyBullet.getY() + 5, 1);

    checkx = EnemyBullet.getX();

    for (_i = 0; _i < 3; _i++) {
      checky = EnemyBullet.getY();
      if (checkx >= Shields[_i].x && checkx <= Shields[_i].x + 15) {
        for (_j = 0; _j <= 5; _j++) {
          if (checky + _j >= Shields[_i].y && checky + _j <= Shields[_i].y + 7) {
            if (GetShieldPixel(checkx, checky + _j, _i, 1) == 1) {
              SetShieldPixel(checkx, checky + _j, _i, 0, 1);
              SetShieldPixel(checkx - 1, checky + _j, _i, 0, 1);
              SetShieldPixel(checkx + 1, checky + _j, _i, 0, 1);
              SetShieldPixel(checkx, checky + _j + 1, _i, 0, 1);

              EnemyBullet.exist = 0;
              break;
            }
          }
        }
      }
    }
  }

  // move enemies every 5th frame
  if (CurrentFrame % 5 == 0) {
    // move enemies
    bounce = 0;
    for (_i = 0; _i < nr_enemy; _i++) {
      if (bounce == 0 && enemy[_i].Alive() == true && enemy[_i].move() == 1) {
        // if one of the enemies hit the side of the screen...
        for (_j = 0; _j < nr_enemy; _j++) {
          if (_j < _i) {
            enemy[_j].update(1); // enemy already moved, move them back and down
          }
          else if (_j > _i) {
            enemy[_j].update(0); // enemy hasn't moved yet, just move them down
          }
        }
        bounce = 1;
      }
    }
  }

  // render enemies
  for (_i = 0; _i < nr_enemy; _i++) {
    if (enemy[_i].Alive()) {
      if (enemy[_i].Defeat()) {
        GAME_STATE = GAME_STATE_DEFEAT;
      }
      if ((int)random(chance_shoot) == 0) { //enemy shoots bullet
        addBullet(enemy[_i].getX() + 8, enemy[_i].getY() + 4, 3, 0);
      }
      RenderAlien(enemy[_i].getX(), enemy[_i].getY(), (_i * 3) % 10);
    }
  }

  // render shields
  for (_i = 0; _i < 3; _i++) {
    for (_x = 0; _x < 16; _x++) {
      for (_y = 0; _y < 8; _y++) {
        if (GetShieldPixel(_x, _y, _i) == 1) {
          Display128x64::PlotPixel(Shields[_i].x + _x, Shields[_i].y + _y, 1);
        }
      }
    }
  }

  // render player
  Display128x64::Blit8(ship.getX(), 120, SPACE_INVADERS_SHIP);

  // render info
  sprintf (StringBuffer, "LVL:%d LVS:%d\x0aSCR:%d", level, ship.getLives(), score);
  Display128x64::BlitText58(0, 0, LCD1602, StringBuffer);

}

void RenderAlien(int x, int y, byte sprite)
{
  byte frame = (CurrentFrame / 6) % 2;
  switch (frame) {
    case 0:
      Display128x64::BlitMask8(x, y, SPACE_INVADERS_SPRITES + sprite * 32, SPACE_INVADERS_SPRITES + sprite * 32);
      Display128x64::BlitMask8(x + 8, y, SPACE_INVADERS_SPRITES + sprite * 32 + 8, SPACE_INVADERS_SPRITES + sprite * 32 + 8);
      break;
    case 1:
      Display128x64::BlitMask8(x, y, SPACE_INVADERS_SPRITES + sprite * 32 + 16, SPACE_INVADERS_SPRITES + sprite * 32 + 16);
      Display128x64::BlitMask8(x + 8, y, SPACE_INVADERS_SPRITES + sprite * 32 + 24, SPACE_INVADERS_SPRITES + sprite * 32 + 24);
      break;
  }
}

void RenderPause()
{
  sprintf(StringBuffer, "PUSH");
  Display128x64::BlitText58N(32, 8, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "BUTTON");
  Display128x64::BlitText58N(2, 16, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "TO START");
  Display128x64::BlitText58N(16, 24, LCD1602, StringBuffer, 1);

  RenderAlien(8, 8, 0);
  RenderAlien(48, 16, 1);
  RenderAlien(0, 24, 5);

  if (InputCAlt == 1 || InputZAlt == 1) {
    level = 1;
    InitLevel();
    GAME_STATE = GAME_STATE_PLAY;
  }
}

void RenderTitle()
{
  for (_j = 0; _j < 128; _j++)
  {
    _x = (_j % 16) * 8;
    _y = (_j / 16) * 8;
    Display128x64::Blit8(_x, _y, SPACE_INVADERS_TITLE + _j * 8);
  }

  if (InputCAlt == 1 || InputZAlt == 1) {
    level = 1;
    InitLevel();
    GAME_STATE = GAME_STATE_PAUSE;
  }
}

void RenderDefeat()
{
  sprintf(StringBuffer, "GAME");
  Display128x64::BlitText58N(32, 8, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "OVER");
  Display128x64::BlitText58N(32, 16, LCD1602, StringBuffer, 1);

  RenderAlien(8, 8, 2);
  RenderAlien(48, 32, 3);

  if (InputCAlt == 1 || InputZAlt == 1) {
    GAME_STATE = GAME_STATE_PAUSE;
  }
}


void RenderDisplay()
{
  Display128x64::ClearDisplay();

  switch (GAME_STATE) {
    case GAME_STATE_PLAY:
      Display128x64::Rotation = SCREEN_PORTRAIT;
      RenderPlay();
      break;
    case GAME_STATE_TITLE:
      Display128x64::Rotation = SCREEN_LANDSCAPE;
      RenderTitle();
      break;
    case GAME_STATE_PAUSE:
      Display128x64::Rotation = SCREEN_PORTRAIT;
      RenderPause();
      break;
    case GAME_STATE_DEFEAT:
      Display128x64::Rotation = SCREEN_PORTRAIT;
      RenderDefeat();
      break;
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
  CurrentFrame++;
}



void loop() {
  ReadController();

  RenderDisplay();


  CurrentFrame++;
  CurrentFrame %= 240;

  delay(25); // each frame is 50 milliseconds
}
