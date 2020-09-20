/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include "Arduino.h"
#include "WeedController.h"

void WeedController::Init() {
  weed_state = WEED_STATE_WALK1_TRANSITION;
  weed_state_time = 0;
  weed_state_duration = 0;
  SetFrame(0);
}

void WeedController::Update(float s) {
  weed_state_time += s;
  if (weed_state_time >= weed_state_duration) {
    NextState();
  }
}

void WeedController::SetFrame(int num) {
  int j;
  switch (num) {
    case 0:
        frame = WeedController_Frame1;
      break;
    case 1:
        frame = WeedController_Frame2;
      break;
    case 2:
        frame = WeedController_Frame3;
      break;
    case 3:
        frame = WeedController_Frame4;
      break;
  }
  weed_sprite_change = true;
}

void WeedController::NextState() {
  switch (weed_state) {
    case WEED_STATE_WALK1_TRANSITION:
      SetFrame(0);
      weed_state = WEED_STATE_WALK1;
      weed_state_duration = 0.250;
      break;
    case WEED_STATE_WALK1:
      weed_state = WEED_STATE_WALK2_TRANSITION;
      weed_state_duration = 0;
      break;
    case WEED_STATE_WALK2_TRANSITION:
      SetFrame(1);
      weed_state = WEED_STATE_WALK2;
      weed_state_duration = 0.250;
      break;
    case WEED_STATE_WALK2:
      weed_state = WEED_STATE_WALK3_TRANSITION;
      weed_state_duration = 0;
      break;
    case WEED_STATE_WALK3_TRANSITION:
      SetFrame(2);
      weed_state = WEED_STATE_WALK3;
      weed_state_duration = 0.250;
      break;
    case WEED_STATE_WALK3:
      weed_state = WEED_STATE_WALK4_TRANSITION;
      weed_state_duration = 0;
      break;
    case WEED_STATE_WALK4_TRANSITION:
      SetFrame(3);
      weed_state = WEED_STATE_WALK4;
      weed_state_duration = 0.250;
      break;
    case WEED_STATE_WALK4:
      weed_state = WEED_STATE_WALK1_TRANSITION;
      weed_state_duration = 0;
      break;
  }

  weed_state_time = 0;
}
