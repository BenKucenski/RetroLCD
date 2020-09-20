/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include <Wire.h>
#include "Display128x64.h"
#include "nunchuk.h"
#include "sprites.h"


#include "DinoController.h"
#include "WeedController.h"
#include "CactiController.h"

char Screen[32];
int screen_x = 16;
int screen_y = 2;
int start_timer;
int end_timer;
float frame_time;
float total_time;
int player_x;
int max_x = 500;
int points = 0;

float weed_x;
float weed_y;

DinoController dino;
WeedController weed;

float cacti_x[10];
int cacti_sprite[10];
int cacti_count = 10;
char StringBuffer[32];

void setup() {
  Wire.begin();

  Nunchuk::NunchukInit();
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  Nunchuk::NunchukRead();
  delay(100);
  randomSeed(analogRead(0));


  dino.Init();
  weed.Init();

  player_x = 0;

  total_time = 0;
  weed_x = random(20, 100);
  int j;
  for (j = 0; j < cacti_count; j++) {
    cacti_x[j] = random(17, max_x);
    cacti_sprite[j] = random(CactiController_frame1, CactiController_frame2 + 1);
  }
}

void ClearScreen() {
  for (int j = 0; j < 32; j++) {
    Screen[j] = ' ';
  }
}

void PlotCHAR(int x, int y, char c) {
  if (x + y * screen_x >= 32) {
    return;
  }
  if (x < 0) {
    return;
  }
  if (y < 0) {
    return;
  }
  if (x > 15) {
    return;
  }
  if (y > 1) {
    return;
  }
  Screen[x + y * screen_x] = c;
}

void RenderScreen() {
  int x, y;
  char line[17];

  Display128x64::ClearDisplay();

  sprintf(StringBuffer, "Pts: %d", points);
  Display128x64::BlitText58(0, 48, Sprites, StringBuffer);


  for (y = 0; y < screen_y; y++) {
    for (x = 0; x < screen_x; x++) {
      line[x] = Screen[x + y * screen_x];
    }
    line[16] = 0;
    Display128x64::BlitText58(0, y * 8 + 16, Sprites, line);
  }



  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();

  delay(33);
}

void HandleInput()
{
  Wire.setClock(CLOCK_NUNCHUK);

  if (!Nunchuk::NunchukRead()) {
    return;
  }

  float InputForceX;
  float InputForceY;

  InputForceX = map(Nunchuk::NunchukX(), -128, 127, 0, 255) * 1.0 / 128.0 - 1.0;

  if (dino.dino_state != DINO_STATE_JUMP
      && dino.dino_state != DINO_STATE_CROUCH) {
    if (Nunchuk::NunchukC()) {
      dino.Jump();
    } else {
      if (Nunchuk::NunchukZ()) {
        dino.Crouch();
      } else {
        if (InputForceX < 0) {
          if (player_x > 0) {
            player_x--;
          }
        } else {
          if (InputForceX > 0) {
            if (player_x < 4) {
              player_x++;
            }
          }
        }
      }
    }
  }
}

void HandleSpriteChange()
{
  // no longer needed since we're not loading to the character display
  if (weed.weed_sprite_change) {
    weed.weed_sprite_change = false;
  }

  if (dino.dino_sprite_change) {
    dino.dino_sprite_change = false;
  }
}

void HandleSpriteUpdates()
{
  weed_x -= frame_time * 4.0;
  if (weed_x < 0) {
    points += 10;
    weed_x = random(200, 1000);
  }
  weed_y = (int)(sin(total_time * 6.0 / 3.14) + 1);

  int j;
  for (j = 0; j < cacti_count; j++) {
    cacti_x[j] -= frame_time * 4.0;
    if (cacti_x[j] < 0) {
      points++;
      cacti_x[j] = random(17, max_x);
      cacti_sprite[j] = random(CactiController_frame1, CactiController_frame2 + 1);
    }
  }
}

void HandleCollision()
{
  switch (dino.dino_state) {
    case DINO_STATE_JUMP:
      if (weed_y == 0)
      {
        if (weed_x >= player_x && weed_x <= player_x + 2) {
          weed_x = random(20, 100);
          points -= 5;
          dino.Hit();
        }
      }
      break;
    case DINO_STATE_CROUCH:
      if (weed_y == 1)
      {
        if (weed_x >= player_x && weed_x <= player_x + 2) {
          weed_x = random(20, 100);
          points -= 5;
          dino.Hit();
        }
      }
      break;
    default:
      if (weed_x >= player_x && weed_x <= player_x + 1) {
        weed_x = random(20, 100);
        points -= 5;
        dino.Hit();
      }
      break;
  }

  if (dino.dino_state != DINO_STATE_JUMP) {
    int j;
    for (j = 0; j < cacti_count; j++) {
      if (player_x == (int)cacti_x[j]) {
        cacti_x[j] = random(17, max_x);
        cacti_sprite[j] = random(CactiController_frame1, CactiController_frame2 + 1);
        points--;
        dino.Hit();
        break;
      }

    }
  }
  if (points < 0) {
    points = 0;
  }
}

void loop() {
  start_timer = millis();

  HandleInput();
  HandleSpriteChange();
  HandleSpriteUpdates();
  HandleCollision();


  ClearScreen();
  PlotCHAR((int) weed_x, (int) weed_y, weed.frame);
  switch (dino.dino_state) {
    case DINO_STATE_JUMP:
      PlotCHAR(player_x + 0, 0, dino.dinoTL);
      PlotCHAR(player_x + 1, 0, dino.dinoTR);
      PlotCHAR(player_x + 2, 0, dino.dinoBL);
      break;
    case DINO_STATE_CROUCH:
      PlotCHAR(player_x + 0, 1, dino.dinoTL);
      PlotCHAR(player_x + 1, 1, dino.dinoTR);
      PlotCHAR(player_x + 2, 1, dino.dinoBL);
      break;
    default:
      PlotCHAR(player_x + 0, 0, dino.dinoTL);
      PlotCHAR(player_x + 1, 0, dino.dinoTR);
      PlotCHAR(player_x + 0, 1, dino.dinoBL);
      PlotCHAR(player_x + 1, 1, dino.dinoBR);
      break;
  }

  int j;
  for (j = 0; j < cacti_count; j++) {
    PlotCHAR((int)cacti_x[j], 1, cacti_sprite[j]);
  }

  RenderScreen();

  end_timer = millis();
  frame_time = (float) (end_timer - start_timer) / 1000.0;
  dino.Update(frame_time);
  weed.Update(frame_time);

  total_time += frame_time;
  total_time = ((total_time / 60.0) - (int) (total_time / 60.0)) * 60.0;

}
