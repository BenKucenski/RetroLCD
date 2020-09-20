/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#ifndef WeedController_h
#define WeedController_h

#define WEED_STATE_WALK1_TRANSITION 1
#define WEED_STATE_WALK1 2

#define WEED_STATE_WALK2_TRANSITION 3
#define WEED_STATE_WALK2 4

#define WEED_STATE_WALK3_TRANSITION 5
#define WEED_STATE_WALK3 6

#define WEED_STATE_WALK4_TRANSITION 7
#define WEED_STATE_WALK4 8

// needs to be define here with the actual value in the h file
#define WeedController_Frame1 7
#define WeedController_Frame2 8
#define WeedController_Frame3 9
#define WeedController_Frame4 10

#include "Arduino.h"

class WeedController {
  private:
    int weed_state;
    float weed_state_time;
    float weed_state_duration;

    void NextState();
    void SetFrame(int num);

  public:

    byte frame;
    bool weed_sprite_change;


    void Init();
    void Update(float s);

};

#endif
