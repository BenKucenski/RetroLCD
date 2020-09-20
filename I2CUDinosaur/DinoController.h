/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#ifndef DinoController_h
#define DinoController_h

#define DINO_STATE_WALK1_TRANSITION 1
#define DINO_STATE_WALK1 2

#define DINO_STATE_WALK2_TRANSITION 3
#define DINO_STATE_WALK2 4

#define DINO_STATE_JUMP 5
#define DINO_STATE_CROUCH 6


// needs to be define here with the actual value in the h file
#define DinoController_dinoHitL 13
#define DinoController_dinoHitR 14

#define DinoController_dinoTL1 2
#define DinoController_dinoTR1 4
#define DinoController_dinoBL1 1
#define DinoController_dinoBR1 3
#define DinoController_dinoBL2 5
#define DinoController_dinoBR2 6

#define DinoController_JumpCrouch1 15
#define DinoController_JumpCrouch2 16
#define DinoController_JumpCrouch3 17

#include "Arduino.h"

class DinoController {
  private:
    float dino_state_time;
    float dino_hit_timer;

    float dino_state_duration;

    void NextState();
    void SetWalk1();
    void SetWalk2();
    void SetJumpCrouch();

  public:
    byte dinoTL;
    byte dinoTR;
    byte dinoBL;
    byte dinoBR;
    bool dino_sprite_change;
    int dino_state;
    bool dino_is_hit;

    void Init();
    void Update(float s);
    void Jump();
    void Crouch();
    void Hit();

};

#endif
