/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#ifndef Space_Invaders_H
#define Space_Invaders_H
// https://github.com/ccostin93/Space-Invaders-library
#include "Arduino.h"

class Ship { //my ship
  private:
    byte x, y; //location
    byte speed; //the speed of the movement
    byte lives; //the number of lives left
  public:
    void Init(byte newX,  byte newY);
    void move(byte direction);
    void setX(byte newX);
    void setY(byte newY);
    byte getX();
    byte getY();
    void setLives(byte newLives);
    byte getLives();
};

class Enemy { //the enemy ships
  private:
    byte x, y; //location
    byte speed; //the speed of the movement
    bool alive; //true - it's alive, false - it is not
  public:
    void Init(byte newX,  byte newY, byte newSpeed);
    char move();
    void update(char ch);
    void setDead();
    byte getX();
    byte getY();
    bool Alive();
    bool Defeat();
};

class Bullet {
  private:
    byte x, y;
    byte speed;
  public:
    char exist;

    void Init(byte newX, byte newY, byte newSpeed);
    void move();
    byte getSpeed();
    byte getX();
    byte getY();
    char getExist();
    char collisionShip(Ship &ship);
    char collisionEnemy(Enemy &enemy);
};

#endif
