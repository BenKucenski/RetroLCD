/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#ifndef BASKETBALL_H
#define BASKETBALL_H

#define CONTROLLER_BUTTON_UP 1
#define CONTROLLER_BUTTON_LEFT 2
#define CONTROLLER_BUTTON_RIGHT 4
#define CONTROLLER_BUTTON_DOWN 8

#define QUARTER_MINUTES 10
#define SHOT_CLOCK_SECONDS 24

#define SCORE_MODE_HOME 0
#define SCORE_MODE_VISITOR 1

PROGMEM const byte BASKETBALL_SPRITES[] = {
  0b00111100, // 0 - ball
  0b01011010,
  0b10100101,
  0b10100101,
  0b10100101,
  0b10100101,
  0b01011010,
  0b00111100,
  0b00111100, // 8 - ball mask
  0b01111110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000, // 16 - defender
  0b00111100,
  0b10111101,
  0b01111110,
  0b00011000,
  0b00011000,
  0b00100100,
  0b01000010,
  0b00011000, // 24 - player
  0b00111100,
  0b00111100,
  0b00011000,
  0b01111110,
  0b00011000,
  0b00100100,
  0b01000010,
  0b11111111, // 32 - basket
  0b11100101,
  0b10011001,
  0b01101010,
  0b01010110,
  0b01101010,
  0b00100100,
  0b00111100
};

class Basketball
{
  private:
    byte last_moved = 0;
    byte ScoreMode;
    byte ShotStep;
    byte ShotFrameCount;
    byte EnemyPosition[5];
    byte CurrentCounter;

    byte BallConversion[12] = {
      3, 19, 1, 18, 32, 33, 34, 65, 50, 67, 51, 35
    };

    byte Shot4[4] = {
      22, 23, 33, 21
    };

    byte Shot20[4] = {
      31, 32, 33, 21
    };

    byte Shot2[4] = {
      24, 25, 33, 21
    };

    byte Shot18[4] = {
      29, 30, 33, 21
    };

    byte Shot9[5] = {
      26, 27, 28, 33, 21
    };

    void PlotPixel(byte x, byte y, byte c);
    byte OpenPosition(byte x, byte y);
    byte CheckPosition(byte x, byte y);
    void MoveEnemies();
    void CheckBall();
    void MovePlayerPosition(byte x, byte y, byte & p);
    void SetPlayerPosition(byte x, byte y, byte & p);
    void ResetEnemyPositions();
    void SwitchSides(byte ShotPoints);

  public:
    byte  *Shot;
    byte Display[5];

    byte ShotTimer;
    byte CurrentQuarterMinutes;
    byte CurrentQuarterSeconds;
    byte CurrentQuarter;

    byte ScoreHome;
    byte ScoreVisitor;

    byte PlayerPosition;
    byte GetPlayerX(byte & p);
    byte GetPlayerY(byte & p);
    byte GetPixel(byte x, byte y);


    byte MovePlayer(byte dir);
    void StartShot(byte & p);

    void ClearScreen();
    void RenderCourt();
    void ResetGame();

    void Update(byte CurrentFrame);
    void HandleShot(byte CurrentFrame);
};

#endif
