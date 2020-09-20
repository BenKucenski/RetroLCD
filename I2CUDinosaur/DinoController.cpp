/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include "Arduino.h"
#include "DinoController.h"

void DinoController::Init() {
  dino_state = DINO_STATE_WALK1_TRANSITION;
  dino_state_time = 0;
  dino_state_duration = 0;
  dino_is_hit = false;
  SetWalk1();
}

void DinoController::Hit() {
  if (dino_is_hit) {
    return;
  }
  dino_state = DINO_STATE_WALK1_TRANSITION;
  dino_state_time = 0;
  dino_state_duration = 0;
  dino_hit_timer = 1.0;
  dino_is_hit = true;
  SetWalk1();
}
void DinoController::Update(float s) {
  dino_state_time += s;
  if (dino_state_time >= dino_state_duration) {
    NextState();
  }
  if (dino_is_hit) {
    dino_hit_timer -= s;
    if (dino_hit_timer <= 0) {
      dino_hit_timer = 0;
      dino_is_hit = false;
    }
  }
}

void DinoController::SetWalk1() {
  if (dino_is_hit) {
    dinoTL = DinoController_dinoHitL;
    dinoTR = DinoController_dinoHitR;
  } else {
    dinoTL = DinoController_dinoTL1;
    dinoTR = DinoController_dinoTR1;
  }
  dinoBL = DinoController_dinoBL1;
  dinoBR = DinoController_dinoBR1;
  dino_sprite_change = true;
}

void DinoController::SetWalk2() {

  if (dino_is_hit) {
    dinoTL = DinoController_dinoHitL;
    dinoTR = DinoController_dinoHitR;
  } else {
    dinoTL = DinoController_dinoTL1;
    dinoTR = DinoController_dinoTR1;
  }
  dinoBL = DinoController_dinoBL2;
  dinoBR = DinoController_dinoBR2;
  dino_sprite_change = true;
}

void DinoController::SetJumpCrouch() {
  if (dino_is_hit) {
    return;
  }
  dinoTL = DinoController_JumpCrouch1;
  dinoTR = DinoController_JumpCrouch2;
  dinoBL = DinoController_JumpCrouch3;
  dino_sprite_change = true;
}

void DinoController::Jump() {
  if (dino_is_hit) {
    return;
  }
  SetJumpCrouch();
  dino_state = DINO_STATE_JUMP;
  dino_state_duration = 1.25;
}

void DinoController::Crouch() {
  if (dino_is_hit) {
    return;
  }
  SetJumpCrouch();
  dino_state = DINO_STATE_CROUCH;
  dino_state_duration = 1.25;
}

void DinoController::NextState() {
  switch (dino_state) {
    case DINO_STATE_WALK1_TRANSITION:
      SetWalk1();
      dino_state = DINO_STATE_WALK1;
      dino_state_duration = 0.250;
      break;
    case DINO_STATE_WALK1:
      dino_state = DINO_STATE_WALK2_TRANSITION;
      dino_state_duration = 0;
      break;
    case DINO_STATE_WALK2_TRANSITION:
      SetWalk2();
      dino_state = DINO_STATE_WALK2;
      dino_state_duration = 0.250;
      break;
    case DINO_STATE_WALK2:
      dino_state = DINO_STATE_WALK1_TRANSITION;
      dino_state_duration = 0;
      break;
    case DINO_STATE_CROUCH:
    case DINO_STATE_JUMP:
      dino_state = DINO_STATE_WALK1_TRANSITION;
      dino_state_duration = 0;
      break;

  }

  dino_state_time = 0;
}
