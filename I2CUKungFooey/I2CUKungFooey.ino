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

#include "Title.h"
#include "Sprites.h"
#include "SpriteRender.h"
#include "Fighter.h"

#define GAME_STATE_PLAY 0
#define GAME_STATE_PAUSE 1
#define GAME_STATE_DEFEAT 2
#define GAME_STATE_TITLE 3
#define GAME_STATE_WIN 4

// graphics stuff
byte CurrentFrame;

// generics
byte _i, _j, _x, _y;

// input stuff
float InputForceX;
float InputForceY;
byte InputC; // can be held down
byte InputZ;
byte InputCAlt; // can't be held down
byte InputZAlt;

char StringBuffer[32];
byte GAME_STATE;
Fighter Player, Enemy;

void setup() {
  Wire.begin();

  Nunchuk::NunchukInit();
  Display128x64::Rotation = SCREEN_LANDSCAPE;
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  Nunchuk::NunchukRead();
  delay(100);
  randomSeed(analogRead(0));

  CurrentFrame = 0;
  ChangeState(GAME_STATE_TITLE);
}

void ChangeState(byte state)
{
  switch (state) {
    case GAME_STATE_PLAY:
      InitGame();
      break;
  }
  GAME_STATE = state;
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

void InitGame()
{
  Player.state = FIGHTER_STATE_STAND;
  Player.dir = FACE_RIGHT;
  Player.x = 8;
  Player.y = 56;
  Player.scene_floor = 56;
  Player.health = 100;

  Enemy.state = FIGHTER_STATE_STAND;
  Enemy.dir = FACE_LEFT;
  Enemy.x = 112;
  Enemy.y = 56;
  Enemy.scene_floor = 56;
  Enemy.health = 100;
}

void RenderTitle()
{
  for (_j = 0; _j < 128; _j++)
  {
    _x = (_j % 16) * 8;
    _y = (_j / 16) * 8;
    Display128x64::Blit8(_x, _y, KF_TITLE + _j * 8);
  }

  if (InputCAlt == 1 || InputZAlt == 1) {
    GAME_STATE = GAME_STATE_PLAY;
    InitGame();
  }

  char DRAGON_FRAME = 0, DRAGON_X = 0;
  if (CurrentFrame < 40) {
    DRAGON_X = (CurrentFrame % 4) - 2;
    if (CurrentFrame < 10) {
      DRAGON_FRAME = FRAME_PUFF_1;
    } else {
      if (CurrentFrame < 20) {
        DRAGON_FRAME = FRAME_PUFF_2;
      } else {
        if (CurrentFrame < 30) {
          DRAGON_FRAME = FRAME_PUFF_3;
        } else {
          DRAGON_FRAME = FRAME_PUFF_4;
        }
      }
    }
  } else {
    if (CurrentFrame < 175) {
      DRAGON_FRAME = FRAME_DRAGON;
    } else {
      DRAGON_X = (CurrentFrame % 4) - 2;
      if (CurrentFrame < 185) {
        DRAGON_FRAME = FRAME_PUFF_4;
      } else {
        if (CurrentFrame < 195) {
          DRAGON_FRAME = FRAME_PUFF_3;
        } else {
          if (CurrentFrame < 205) {
            DRAGON_FRAME = FRAME_PUFF_2;
          } else {
            if (CurrentFrame < 215) {
              DRAGON_FRAME = FRAME_PUFF_1;
            }
          }
        }
      }

    }
  }

  if (DRAGON_FRAME != 0) {
    SpriteRender::RenderFrame(DRAGON_X + 8, 4, 0, DRAGON_FRAME);
    SpriteRender::RenderFrame(128 - DRAGON_X - 8 - 8, 4, 1, DRAGON_FRAME);
  }

  //CurrentFrame = 100;
  if (CurrentFrame >= 100 && CurrentFrame <= 200) {
    if (CurrentFrame < 160) {
      byte len = (CurrentFrame - 100) % 5 + 1;
      SpriteRender::RenderFlame(22, 0, 0, len);
      SpriteRender::RenderFlame(128 - 22, 0, 1, len);
    } else {
      SpriteRender::RenderFlame((CurrentFrame - 160) * 4 + 22, 0, 0, 0);
      SpriteRender::RenderFlame(128 - 22 - (CurrentFrame - 160) * 4, 0, 1, 0);
    }
  }


  sprintf(StringBuffer, "Press Button");
  Display128x64::BlitText58N((128 - (12 * 6)) / 2, 29, LCD1602, StringBuffer, 1);

  //sprintf(StringBuffer, "%d %d", CurrentFrame, DRAGON_X);
  //Display128x64::BlitText58N(0, 56, LCD1602, StringBuffer, 1);
}

void RenderPlay()
{
  Player.Render();
  Enemy.Render();

  Player.Update(InputForceX, InputForceY, InputCAlt, InputZAlt, InputC, InputZ);
  Enemy.Update(Player);
  Enemy.AI(Player);

  Fighter::HitCheck(Player, Enemy);
  Fighter::HitCheck(Enemy, Player);

  if (Player.x < 8) {
    Player.x = 8;
  }

  if (Player.x > 112) {
    Player.x = 112;
  }

  if (Enemy.x < 8) {
    Enemy.x = 8;
  }

  if (Enemy.x > 112) {
    Enemy.x = 112;
  }

  if(Enemy.health == 0) {
    ChangeState(GAME_STATE_WIN);
  } else {
    if(Player.health == 0) {
      ChangeState(GAME_STATE_DEFEAT);
    }
  }

  byte screen_x;
  screen_x = Player.health / 2;
  Display128x64::DrawLine(0, 1, screen_x, 1, 1);
  Display128x64::DrawLine(0, 2, screen_x, 2, 1);
  Display128x64::DrawLine(0, 3, screen_x, 3, 1);

  screen_x = 127 - Enemy.health / 2;
  Display128x64::DrawLine(127, 1, screen_x, 1, 1);
  Display128x64::DrawLine(127, 2, screen_x, 2, 1);
  Display128x64::DrawLine(127, 3, screen_x, 3, 1);


  // floor
  Display128x64::DrawLine(0, 57, 127, 57, 1);
  Display128x64::DrawLine(0, 58, 127, 58, 1);
  Display128x64::DrawLine(0, 60, 127, 60, 1);

  // health border
  Display128x64::DrawLine(0, 0, 50, 0, 1);
  Display128x64::DrawLine(0, 4, 50, 4, 1);
  Display128x64::DrawLine(50, 0, 50, 4, 1);

  Display128x64::DrawLine(127, 0, 77, 0, 1);
  Display128x64::DrawLine(127, 4, 77, 4, 1);
  Display128x64::DrawLine(77, 0, 77, 4, 1);

  //sprintf(StringBuffer, "%d %d %d %d %d", Enemy.x, Enemy.x_dir, Player.x, Player.health, Enemy.health);
  //Display128x64::BlitText58N(0, 56, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "Player");
  Display128x64::BlitText58N(0, 5, LCD1602, StringBuffer, 1);

  sprintf(StringBuffer, "Enemy");
  Display128x64::BlitText58N(127 - 30, 5, LCD1602, StringBuffer, 1);

}

byte d_x;
byte d_y;
byte d_x_s = 3;
byte d_y_s = 1;

void RenderDefeat()
{
  if (random(0, 3) == 0) {
    sprintf(StringBuffer, "You Loser!");
  } else {
    sprintf(StringBuffer, "You Lose!");
  }
  d_x += d_x_s;
  d_y += d_y_s;
  d_x %= 128;
  d_y %= 64;
  if (d_x < 4) {
    d_x_s = 3;
  }
  if (d_x > 60) {
    d_x_s = -3;
  }
  if (d_y < 4) {
    d_y_s = 1;
  }
  if (d_y > 52) {
    d_y_s = -1;
  }
  Display128x64::BlitText58N(d_x, d_y, LCD1602, StringBuffer, 1);
  SpriteRender::RenderFrame(20, 20, 1, FRAME_DEATH_2);

  if (InputCAlt == 1 || InputZAlt == 1) {
    ChangeState(GAME_STATE_TITLE);
  }
}

void RenderWin()
{
  if (random(0, 3) == 0) {
    sprintf(StringBuffer, "You Winner!");
  } else {
    sprintf(StringBuffer, "You Win!");
  }
  d_x += d_x_s;
  d_y += d_y_s;
  d_x %= 128;
  d_y %= 64;
  if (d_x < 4) {
    d_x_s = 3;
  }
  if (d_x > 60) {
    d_x_s = -3;
  }
  if (d_y < 4) {
    d_y_s = 1;
  }
  if (d_y > 52) {
    d_y_s = -1;
  }
  Display128x64::BlitText58N(d_x, d_y, LCD1602, StringBuffer, 1);
  SpriteRender::RenderFrame(20, 20, 1, FRAME_HIGH_KICK_2);

  SpriteRender::RenderFrame(40, 10, 0, FRAME_DEATH_2);

  if (InputCAlt == 1 || InputZAlt == 1) {
    ChangeState(GAME_STATE_TITLE);
  }
}

void RenderDisplay()
{
  Display128x64::ClearDisplay();

  switch (GAME_STATE) {
    case GAME_STATE_TITLE:
      RenderTitle();
      break;
    case GAME_STATE_PLAY:
      RenderPlay();
      break;
    case GAME_STATE_DEFEAT:
      RenderDefeat();
      break;
    case GAME_STATE_WIN:
      RenderWin();
      break;
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
}


void loop() {
  ReadController();

  RenderDisplay();

  CurrentFrame++;
  CurrentFrame %= 240;

  delay(33); // each frame is 50 milliseconds
}
