#define FRAME_STAND 0
#define FRAME_WALK 1
#define FRAME_BLOCK 2
#define FRAME_MID_KICK_1 2
#define FRAME_MID_KICK_2 3
#define FRAME_MID_KICK_3 4
#define FRAME_MID_PUNCH_1 2
#define FRAME_MID_PUNCH_2 5
#define FRAME_LOW_BLOCK 6
#define FRAME_LOW_PUNCH 7
#define FRAME_CROUCH 8
#define FRAME_LOW_KICK_1 8
#define FRAME_LOW_KICK_2 9
#define FRAME_HIGH_KICK_1 10
#define FRAME_HIGH_KICK_2 11
#define FRAME_STAIR 12
#define FRAME_DEATH_1 13
#define FRAME_DEATH_2 14

#define FRAME_PUFF_1 15
#define FRAME_PUFF_2 16
#define FRAME_PUFF_3 17
#define FRAME_PUFF_4 18

#define FRAME_DRAGON 19


#define MAX_FRAMES 15
class SpriteRender {
  private:
    // the "inline" keyword makes it possible to use the variables inside the H file.  Otherwise, you have to declare them in the .cpp file
    inline static char RenderDirection;
    inline static char i;
    inline static char j;
    inline static char k;

  public:

    static void RenderSprite(int number, int x, int y, byte flip)
    {
      if (flip == 0) {
        Display128x64::BlitMask8(x, y, KF_SPRITES + number * 8, KF_SPRITES + number * 8);
      } else {
        Display128x64::BlitMask8FlipH(x, y, KF_SPRITES + number * 8, KF_SPRITES + number * 8);
      }
    }

    static void RenderFrame(int x, int y, byte flip, byte frame)
    {
      RenderDirection = flip == 0 ? 8 : -8;
      x -= flip == 0 ? 4 : -4;

      switch (frame) {
        case FRAME_PUFF_1:
          y -= 12;
          RenderRange(x, y, flip, 40, 41, 3);
          break;
        case FRAME_PUFF_2:
          y -= 4;
          RenderRange(x, y, flip, 42, 43, 5);
          break;
        case FRAME_PUFF_3:
          y -= 4;
          RenderRange(x, y, flip, 44, 45, 5);
          break;
        case FRAME_PUFF_4:
          y -= 4;
          RenderRange(x, y, flip, 46, 48, 5);
          break;
        case FRAME_DRAGON:
          y -= 8; // undo the normal vertical adjustment
          RenderRange(x, y, flip, 49, 51, 4);
          break;
        case FRAME_STAND:
          RenderRange(x, y, flip, 0, 1, 5);
          break;
        case FRAME_WALK:
          RenderRange(x, y, flip, 2, 3, 5);
          break;
        case FRAME_BLOCK:
          // case FRAME_MID_KICK_1:
          // case FRAME_MID_PUNCH_1:
          RenderRange(x, y, flip, 4, 5, 5);
          break;
        case FRAME_MID_KICK_2:
          RenderRange(x, y, flip, 6, 7, 5);
          break;
        case FRAME_MID_KICK_3:
          RenderRange(x, y, flip, 8, 11, 5);
          break;
        case FRAME_MID_PUNCH_2:
          RenderRange(x, y, flip, 12, 14, 5);
          break;
        case FRAME_LOW_BLOCK:
          RenderRange(x, y, flip, 15, 17, 4);
          break;
        case FRAME_LOW_PUNCH:
          RenderRange(x, y, flip, 18, 21, 4);
          break;
        case FRAME_LOW_KICK_1:
          RenderRange(x, y, flip, 22, 23, 4);
          break;
        case FRAME_LOW_KICK_2:
          RenderRange(x, y, flip, 24, 27, 4);
          break;
        case FRAME_HIGH_KICK_1:
          RenderRange(x, y, flip, 28, 29, 5);
          break;
        case FRAME_HIGH_KICK_2:
          RenderRange(x, y, flip, 30, 32, 5);
          break;
        case FRAME_STAIR:
          RenderRange(x, y, flip, 33, 34, 5);
          break;
        case FRAME_DEATH_1:
          RenderRange(x, y, flip, 35, 36, 5);
          break;
        case FRAME_DEATH_2:
          RenderRange(x, y, flip, 37, 39, 5);
          break;


      }
    }

    static void RenderRange(int x, int y, byte flip, byte first, byte last, byte height)
    {
      x -= last - first > 1 ? (last - first - 1) * RenderDirection : 0;
      for (i = 5 - height; i < 5; i++) {
        k = 0;
        for (j = first; j <= last; j++) {
          RenderSprite(j + i * 56, x + RenderDirection * k, y + 8 * i, flip);
          k++;
        }
      }
    }




    static void RenderFlame(int x, int y, byte flip, byte len)
    {
      if (flip == 0) {
        SpriteRender::RenderSprite(52 + 3 * 56, x + 0, y + 0, flip);
        SpriteRender::RenderSprite(52 + 4 * 56, x + 0, y + 8, flip);
        if (len == 1) {
          return;
        }

        SpriteRender::RenderSprite(53 + 3 * 56, x + 8, y + 0, flip);
        SpriteRender::RenderSprite(53 + 4 * 56, x + 8, y + 8, flip);
        if (len == 2) {
          return;
        }

        SpriteRender::RenderSprite(54 + 3 * 56, x + 16, y + 0, flip);
        SpriteRender::RenderSprite(54 + 4 * 56, x + 16, y + 8, flip);
        if (len == 3) {
          return;
        }

        SpriteRender::RenderSprite(55 + 3 * 56, x + 24, y + 0, flip);
        SpriteRender::RenderSprite(55 + 4 * 56, x + 24, y + 8, flip);
      } else {
        SpriteRender::RenderSprite(52 + 3 * 56, x - 8, y + 0, flip);
        SpriteRender::RenderSprite(52 + 4 * 56, x - 8, y + 8, flip);
        if (len == 1) {
          return;
        }

        SpriteRender::RenderSprite(53 + 3 * 56, x - 16, y + 0, flip);
        SpriteRender::RenderSprite(53 + 4 * 56, x - 16, y + 8, flip);

        if (len == 2) {
          return;
        }
        SpriteRender::RenderSprite(54 + 3 * 56, x - 24, y + 0, flip);
        SpriteRender::RenderSprite(54 + 4 * 56, x - 24, y + 8, flip);
        if (len == 3) {
          return;
        }

        SpriteRender::RenderSprite(55 + 3 * 56, x - 32, y + 0, flip);
        SpriteRender::RenderSprite(55 + 4 * 56, x - 32, y + 8, flip);

      }
    }
};
