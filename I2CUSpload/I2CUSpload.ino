#include <Wire.h>
#include "nunchuk.h"
#include "Display128x64.h"
#include "Codepage437.h"
#include "LCD1602.h"

float InputForceX;
float InputForceY;

int16_t NunchukX_;
int16_t NunchukY_;

float dt;


byte StringBuffer[32];
byte AlertBuffer[32];

unsigned long frame_time;

struct circle {
    byte x;
    byte y;
    byte start_radius;
    byte end_radius;
    float current_radius;
    float current_radius_sq;
    byte expand_speed;
    
};

circle map_circles[5];
circle map_holes[5];

byte c;
unsigned long dist;

void setup()
{
  Wire.begin();

  NunchukInit();
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  NunchukRead();
  delay(100);
  randomSeed(analogRead(0));

  map_circles[0].x = 0;
  map_circles[0].y = 48;
  map_circles[0].current_radius = 12;

  map_circles[1].x = 25;
  map_circles[1].y = 40;
  map_circles[1].current_radius = 16;

  map_circles[2].x = 50;
  map_circles[2].y = 50;
  map_circles[2].current_radius = 14;

  map_circles[3].x = 75;
  map_circles[3].y = 35;
  map_circles[3].current_radius = 18;

  map_circles[4].x = 100;
  map_circles[4].y = 45;
  map_circles[4].current_radius = 25;
  

  map_holes[0].x = 64;
  map_holes[0].y = 16;
  map_holes[0].current_radius = 25;

  map_holes[1].x = 16;
  map_holes[1].y = 24;
  map_holes[1].current_radius = 15;
  
  map_holes[2].x = 100;
  map_holes[2].y = 48;
  map_holes[2].current_radius = 12;

  CalcMapCircles();
  CalcMapHoles();
}

void CalcMapCircles()
{
  for(c = 0; c < 5; c++) {
    if(map_circles[c].end_radius == 0) {
      continue;
    }
    map_circles[c].current_radius_sq = map_circles[c].current_radius * map_circles[c].current_radius;
  }  
}

void CalcMapHoles()
{
  for(c = 0; c < 5; c++) {
    if(map_holes[c].end_radius == 0) {
      continue;
    }
    map_holes[c].current_radius_sq = map_holes[c].current_radius * map_holes[c].current_radius;
  }  
}

byte inMapHole(byte sx, byte sy) 
{
  for(c = 0; c < 5; c++) {
    if(map_holes[c].end_radius == 0) {
      continue;
    }
    dist = (sx - map_holes[c].x) * (sx - map_holes[c].x) + (sy - map_holes[c].y) * (sy - map_holes[c].y);
    if(dist < map_holes[c].current_radius_sq) {
      return 1;
    }
  }
  return 0;
}

byte inMapCircle(byte sx, byte sy) 
{
  if(inMapHole(sx, sy)) {
    return 0;
  }
  
  for(c = 0; c < 5; c++) {
    if(map_circles[c].current_radius == 0) {
      continue;
    }
    dist = (sx - map_circles[c].x) * (sx - map_circles[c].x) + (sy - map_circles[c].y) * (sy - map_circles[c].y);
    if(dist < map_circles[c].current_radius_sq) {
      return 1;
    }
  }
  return 0;
}

void ProcessInput(float dt)
{
  Wire.setClock(CLOCK_NUNCHUK);

  if (!NunchukRead()) {
    return;
  }
  NunchukX_ = map(NunchukX(), -127, 128, 0, 255);
  NunchukY_ = map(NunchukY(), -127, 128, 0, 255);
  
  InputForceX = NunchukX_ * 1.0 / 127.0 - 1.0;
  InputForceY = NunchukY_ * 1.0 / 127.0 - 1.0;
}

void CheckCollisions()
{
}

byte sx;
byte sy;

void loop()
{
  frame_time = micros();

  Display128x64::ClearDisplay();


  sprintf (StringBuffer, "Joystick: %d.%d %d", (char)(InputForceX * 100), (char)(InputForceY * 100), (byte)(dt * 1000));
  Display128x64::BlitText58(0, 0, LCD1602, StringBuffer);

  for(sx =0; sx < 128; sx++) {
    for(sy = 10; sy < 64; sy++) {
      if(inMapCircle(sx, sy) == 1) {
        Display128x64::PlotPixel(sx, sy, 1);
      }
    }
  }

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();

  frame_time = micros() - frame_time;
  dt = (float)(frame_time / 1000) /  1000.0f;
  
  CheckCollisions();
  ProcessInput(dt);

}
