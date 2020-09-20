/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include "Tetris.h"

Tetris::~Tetris() {
  if (playfield) {
    delete[] playfield;
  }
}

void Tetris::Init(int width, int height) {
  board_w = width;
  board_h = height;

  playfield = new byte[width * height];
  BlockSprites[0].top = (byte) B01000100; // I
  BlockSprites[0].bottom = (byte) B01000100;

  BlockSprites[1].top = (byte) B11000000; // L
  BlockSprites[1].bottom = (byte) B01000100;

  BlockSprites[2].top = (byte) B01100000; // backwards L
  BlockSprites[2].bottom = (byte) B01000100;

  BlockSprites[3].top = (byte) B01100000; // square
  BlockSprites[3].bottom = (byte) B00000110;

  BlockSprites[4].top = (byte) B11000000; // z piece
  BlockSprites[4].bottom = (byte) B00000110;

  BlockSprites[5].top = (byte) B01000000; // T piece
  BlockSprites[5].bottom = (byte) B00001110;

  BlockSprites[6].top = (byte) B01100000; // s piece
  BlockSprites[6].bottom = (byte) B00001100;

}

void Tetris::ClearScreen() {
  memset(playfield, 0, board_w * board_h);
  return;
}

int Tetris::CheckCompletedRows() {
  int points = 0;
  int h, w, complete;
  int h2;

  for (h = 0; h < board_h; h++) {
    complete = true;
    for (w = 0; w < board_w; w++) {
      if (playfield[w + h * board_w] == 0) {
        complete = false;
        break;
      }
    }
    if (complete) {
      points++;
      for (h2 = h; h2 > 0; h2--) {
        for (w = 0; w < board_w; w++) {
          playfield[w + h2 * board_w] = playfield[w
                                                  + (h2 - 1) * board_w];
        }
      }
    }
  }

  return points;
}

bool Tetris::GetPiecePixel(int x, int y, TetrisPiece piece) {
  byte i = y > 1 ? piece.bottom : piece.top;
  int j = y % 2 == 1 ? x + 4 : x;
  return (i >> j) & 1;
}

void Tetris::StickPiece(int x, int y, TetrisPiece piece) {
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (x + i < 0) {
        continue;
      }
      if (x + i >= board_w) {
        continue;
      }
      if (y + j < 0) {
        continue;
      }
      if (y + j >= board_h) {
        continue;
      }
      if (GetPiecePixel(i, j, piece)) {
        playfield[x + i + (j + y) * board_w] = 1;
      }
    }
  }
}

bool Tetris::CheckBottomCollision(int x, int y, TetrisPiece piece) {
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (GetPiecePixel(i, j, piece)) {
        if (x + i < 0) {
          continue;
        }
        if (x + i >= board_w) {
          continue;
        }
        if (y + j < 0) {
          continue;
        }
        if (y + j + 1 >= board_h) {
          return true;
        }
        if (playfield[x + i + (y + j + 1) * board_w]) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Tetris::CheckScreenCollision(int x, int y, TetrisPiece piece) {
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (x + i < 0) {
        continue;
      }
      if (x + i >= board_w) {
        continue;
      }
      if (y + j < 0) {
        continue;
      }
      if (y + j >= board_h) {
        continue;
      }
      if (GetPiecePixel(i, j, piece)
          && playfield[x + i + (y + j) * board_w]) {
        return true;
      }
    }
  }
  return false;
}

bool Tetris::CheckCollision(int x, int y, TetrisPiece piece) {
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (GetPiecePixel(i, j, piece)) {
        if (x + i < 0) {
          return true;
        }
        if (x + i >= board_w) {
          return true;
        }
        if (y + j < 0) {
          return true;
        }
        if (y + j >= board_h) {
          return true;
        }
        if (playfield[x + i + (y + j) * board_w]) {
          return true;
        }
      }
    }
  }
  return false;
}

int Tetris::MoveLeft(int x, int y, TetrisPiece piece) {
  return CheckCollision(x - 1, y, piece) ? x : x - 1;
}

int Tetris::MoveRight(int x, int y, TetrisPiece piece) {
  return CheckCollision(x + 1, y, piece) ? x : x + 1;
}

TetrisPiece Tetris::RotateRight(int x, int y, TetrisPiece piece) {
  TetrisPiece temp;
  temp.top = (((piece.bottom >> 4) & 1) << 0) // 0,3
             | (((piece.bottom >> 0) & 1) << 1) // 0,2
             | (((piece.top >> 4) & 1) << 2) // 0,1
             | (((piece.top >> 0) & 1) << 3) // 0,0
             | (((piece.bottom >> 5) & 1) << 4) // 1,3
             | (((piece.bottom >> 1) & 1) << 5) // 1,2
             | (((piece.top >> 5) & 1) << 6) // 1,1
             | (((piece.top >> 1) & 1) << 7) // 1,0
             ;
  temp.bottom = (((piece.bottom >> 6) & 1) << 0) // 2,3
                | (((piece.bottom >> 2) & 1) << 1) // 2,2
                | (((piece.top >> 6) & 1) << 2) // 2,1
                | (((piece.top >> 2) & 1) << 3) // 2,0
                | (((piece.bottom >> 7) & 1) << 4) // 3,3
                | (((piece.bottom >> 3) & 1) << 5) // 3,2
                | (((piece.top >> 7) & 1) << 6) // 3,1
                | (((piece.top >> 3) & 1) << 7) // 3,0
                ;
  return CheckCollision(x, y, temp) ? piece : temp;
}

TetrisPiece Tetris::RotateLeft(int x, int y, TetrisPiece piece) {
  TetrisPiece temp;
  temp.top = (((piece.top >> 3) & 1) << 0) // 3,0
             | (((piece.top >> 7) & 1) << 1) // 3,1
             | (((piece.bottom >> 3) & 1) << 2) // 3,2
             | (((piece.bottom >> 7) & 1) << 3) // 3,3
             | (((piece.top >> 2) & 1) << 4) // 2,0
             | (((piece.top >> 6) & 1) << 5) // 2,1
             | (((piece.bottom >> 2) & 1) << 6) // 2,2
             | (((piece.bottom >> 6) & 1) << 7) // 2,3
             ;
  temp.bottom = (((piece.top >> 1) & 1) << 0) // 1,0
                | (((piece.top >> 5) & 1) << 1) // 1,1
                | (((piece.bottom >> 1) & 1) << 2) // 1,2
                | (((piece.bottom >> 5) & 1) << 3) // 1,3
                | (((piece.top >> 0) & 1) << 4) // 0,0
                | (((piece.top >> 4) & 1) << 5) // 0,1
                | (((piece.bottom >> 0) & 1) << 6) // 0,2
                | (((piece.bottom >> 4) & 1) << 7) // 0,3
                ;
  return CheckCollision(x, y, temp) ? piece : temp;
}
