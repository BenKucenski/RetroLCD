/**
  RetroLCD.com
  Ben Kucenski 2020
  bkucenski@gmail.com
 **/
#include "Space_Invaders.h" //include the declaration for this class
// https://github.com/ccostin93/Space-Invaders-library
void Ship::Init(byte newX,  byte newY) {
  x = newX;
  y = newY;
  speed = 3;
  lives = 3;
}

void Ship::move(byte direction) {
  x += speed * direction;
  if (x < 4) {
    x = 4;
  }
  else if (x > 52) {
    x = 52;
  }
}
void Ship::setX(byte newX) {
  x = newX;
}
void Ship::setY(byte newY) {
  y = newY;
}
byte Ship::getX() {
  return x;
}
byte Ship::getY() {
  return y;
}
void Ship::setLives(byte newLives) {
  lives = newLives;
}
byte Ship::getLives() {
  return lives;
}




void Enemy::Init( byte newX,  byte newY, byte newSpeed) {
  x = newX;
  y = newY;
  alive = true;
  speed = -2 - newSpeed;
}

char Enemy::move() {
  x += speed;
  if (x < 0 || x > 50) {
    speed *= -1;
    x += speed;
    y += 2;
    return 1;
  }
  return 0;
}
void Enemy::update(char ch) {
  speed *= -1;
  y += 2;
  if (ch == 1) {
    x += speed;
  }
}
void Enemy::setDead() {
  alive = false;
}

uint8_t Enemy::getX() {
  return x;
}
uint8_t Enemy::getY() {
  return y;
}
bool Enemy::Alive() {
  return alive;
}
bool Enemy::Defeat() {
  return y >= 112;
}

void Bullet::Init(byte newX, byte newY, byte newSpeed) {
  x = newX;
  y = newY;
  speed = newSpeed;
  exist = 1;
}

void Bullet::move() {
  y += speed;
  if (y < 0 || y > 128) {
    exist = 0; //outside LCD margin
  }
}
byte Bullet::getSpeed() {
  return speed;
}

byte Bullet::getX() {
  return x;
}
byte Bullet::getY() {
  return y;
}

char Bullet::getExist() {
  return exist;
}

char Bullet::collisionShip(Ship &ship) {
  if (x >= ship.getX() && x <= ship.getX() + 7
      && y >= ship.getY() && y <= ship.getY() + 8) { //collision detected
    exist = 0;
    return 1;
  }
  return 0;
}

char Bullet::collisionEnemy(Enemy &enemy) {
  if (enemy.Alive() && x <= enemy.getX() + 15 && x >= enemy.getX()
      && y >= enemy.getY() - 7 && y <= enemy.getY()) { //collison
    exist = 0;
    return 1;
  }
  return 0;
}
