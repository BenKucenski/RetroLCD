#ifndef SPRITECONTROLLER_H
#define SPRITECONTROLLER_H


class SpriteController {
  public:
    float XPos;
    float YPos;

    float TempX;
    float TempY;

    float XForce;
    float YForce;

    byte Sprite;
    byte Direction;

    void Render()
    {
      if (Direction == PLAYER_FACE_RIGHT) {
        Display128x64::BlitMask8((byte)XPos, (byte)YPos, Sprites + Sprite, Sprites + Sprite);
      } else {
        Display128x64::BlitMask8FlipH((byte)XPos, (byte)YPos, Sprites + Sprite, Sprites + Sprite);
      }
    }
};

#endif SPRITECONTROLLER_H
