/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#ifndef TETRIS_H_
#define TETRIS_H_

#include "Arduino.h"

PROGMEM const byte SPRITE_TETRIS_BLOCK[] = {
  0b11111111,
  0b11111111,
  0b11000011,
  0b11011011,
  0b11011011,
  0b11000011,
  0b11111111,
  0b11111111
};

class TetrisPiece
{
  public:
    byte top;
    byte bottom;
};

class Tetris
{
  private:
    int board_w;
    int board_h;

  public:
    byte *playfield;
    TetrisPiece BlockSprites[7];

    ~Tetris();
    void Init(int width, int height);

    void ClearScreen();

    int CheckCompletedRows();

    bool GetPiecePixel(int x, int y, TetrisPiece piece);

    void Render(int PlayerX, int PlayerY, TetrisPiece piece);

    void StickPiece(int x, int y, TetrisPiece piece);

    bool CheckBottomCollision(int x, int y, TetrisPiece piece);
    bool CheckScreenCollision(int x, int y, TetrisPiece piece);
    bool CheckCollision(int x, int y, TetrisPiece piece);

    int MoveLeft(int x, int y, TetrisPiece piece);
    int MoveRight(int x, int y, TetrisPiece piece);

    TetrisPiece RotateRight(int x, int y, TetrisPiece piece);
    TetrisPiece RotateLeft(int x, int y, TetrisPiece piece);

};



#endif /* TETRIS_H_ */
