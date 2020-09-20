#define FIGHTER_STATE_STAND 0
#define FIGHTER_STATE_WALK 1
#define FIGHTER_STATE_PUNCH_MID 2
#define FIGHTER_STATE_KICK_MID 3
#define FIGHTER_STATE_CROUCH 4
#define FIGHTER_STATE_PUNCH_LOW 5
#define FIGHTER_STATE_KICK_LOW 6

#define FIGHTER_STATE_JUMP 7
#define FIGHTER_STATE_KICK_HIGH 8

#define FIGHTER_STATE_HIT 9

#define FIGHTER_STATE_BLOCK_LOW 10
#define FIGHTER_STATE_BLOCK_MID 11

#define FACE_LEFT 0
#define FACE_RIGHT 1
#define DEFENSE_DISTANCE 32
#define OFFENSE_DISTANCE 8

class Fighter
{
  public:
    char x, y;
    byte state;
    byte frame;
    byte state_counter;
    char x_dir;
    char y_dir;
    char dir;
    byte scene_floor;
    byte health;
    byte is_aggressive;
    byte safe_space;


    void ChangeState(byte s)
    {
      state_counter = 0;
      state = s;
    }

    void AI(Fighter &target)
    {
      if (state == FIGHTER_STATE_CROUCH) {
        if (target.state == FIGHTER_STATE_STAND) {
          ChangeState(FIGHTER_STATE_STAND);
        }
      }
      if (state != FIGHTER_STATE_STAND && state != FIGHTER_STATE_WALK && state != FIGHTER_STATE_CROUCH) {
        return;
      }
      if (target.x < x) {
        dir = FACE_LEFT;
      } else {
        dir = FACE_RIGHT;
      }

      if (random(0, 5) > 2) { // only succeed at counter move 40% of the time
        return;
      }

      if (abs(x - target.x) > 32) {
        is_aggressive = 1;
      }
      byte short_range = abs(x - target.x) < 12 ? 1 : 0;
      byte long_range = abs(x - target.x) < 20 ? 1 : 0;


      switch (target.state) {

        case FIGHTER_STATE_STAND:
        case FIGHTER_STATE_WALK:
          safe_space = is_aggressive ? OFFENSE_DISTANCE : DEFENSE_DISTANCE;
          if (target.x < x - safe_space || target.x > x + safe_space) {
            ChangeState(FIGHTER_STATE_WALK);
          } else {
            is_aggressive = 1;
            if (long_range) {
              if (random(0, 2) == 0) {
                ChangeState(FIGHTER_STATE_KICK_MID);
              } else {
                ChangeState(FIGHTER_STATE_KICK_LOW);
              }
            } else {
              switch (random(0, 4)) {
                case 0:
                  ChangeState(FIGHTER_STATE_KICK_MID);
                  break;
                case 1:
                  ChangeState(FIGHTER_STATE_KICK_LOW);
                  break;
                case 2:
                  ChangeState(FIGHTER_STATE_PUNCH_MID);
                  break;
                case 3:
                  ChangeState(FIGHTER_STATE_PUNCH_LOW);
                  break;
              }
            }

          }
          break;
        
        case FIGHTER_STATE_CROUCH:
          safe_space = is_aggressive ? OFFENSE_DISTANCE : DEFENSE_DISTANCE;
          if (target.x < x - safe_space || target.x > x + safe_space) {
            ChangeState(FIGHTER_STATE_WALK);
          } else {
            is_aggressive = 1;
            if (long_range) {
              ChangeState(FIGHTER_STATE_KICK_LOW);
            } else {
              switch (random(0, 2)) {
                case 0:
                  ChangeState(FIGHTER_STATE_PUNCH_LOW);
                  break;
                case 1:
                  ChangeState(FIGHTER_STATE_KICK_LOW);
                  break;
              }
            }

          }
          break;

        case FIGHTER_STATE_PUNCH_MID:
          ChangeState(FIGHTER_STATE_BLOCK_MID);
          break;

        case FIGHTER_STATE_KICK_MID:
          ChangeState(FIGHTER_STATE_BLOCK_MID);
          break;


        case FIGHTER_STATE_PUNCH_LOW:
          ChangeState(FIGHTER_STATE_BLOCK_LOW);
          break;

        case FIGHTER_STATE_KICK_LOW:
          ChangeState(FIGHTER_STATE_BLOCK_LOW);
          break;

        case FIGHTER_STATE_JUMP :
          ChangeState(FIGHTER_STATE_CROUCH);
          break;

        case FIGHTER_STATE_KICK_HIGH:
          ChangeState(FIGHTER_STATE_CROUCH);
          break;

        case FIGHTER_STATE_HIT:
          break;
      }
    }

    void Update(Fighter &target)
    {
      state_counter++;
      switch (state) {
        case FIGHTER_STATE_BLOCK_MID:
          frame = FRAME_BLOCK;
          if (state_counter > 3) {
            ChangeState(FIGHTER_STATE_STAND);
          }
          break;

        case FIGHTER_STATE_BLOCK_LOW:
          frame = FRAME_LOW_BLOCK;
          if (state_counter > 3) {
            ChangeState(FIGHTER_STATE_CROUCH);
          }
          break;

        case FIGHTER_STATE_HIT:
          switch (state_counter) {
            case 1:
              frame = FRAME_DEATH_1;
              break;
            case 2:
              frame = FRAME_DEATH_2;
              break;
          }
          x += x_dir;
          if (y < scene_floor) {
            y += 2;
          } else {
            y = scene_floor;
          }
          if (state_counter > 6) {
            x_dir = 0;
            ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_JUMP:
        case FIGHTER_STATE_KICK_HIGH:
          switch (state_counter) {
            case 1:
              frame = FRAME_HIGH_KICK_1;
              break;
            default:
              frame = FRAME_HIGH_KICK_1;
          }
          if (state_counter < 5) {
            // determine whether to kick or not
          }
          if (state_counter <= 5) {
            y -= 2;
          } else {
            if (y < scene_floor) {
              y += 2;
            } else {
              y = scene_floor;
            }
          }
          if (state_counter >= 10) {
            x_dir = 0;
            ChangeState(FIGHTER_STATE_STAND);
          }
          x += x_dir;
          break;

        case FIGHTER_STATE_CROUCH:
          frame = FRAME_CROUCH;
          break;
        case FIGHTER_STATE_PUNCH_LOW:
          switch (state_counter) {
            case 1:
              frame = FRAME_LOW_BLOCK;
              break;
            case 2:
              frame = FRAME_LOW_PUNCH;
              break;
            case 3:
              frame = FRAME_LOW_BLOCK;
              break;
            default:
              ChangeState(FIGHTER_STATE_CROUCH);
          }
          break;
        case FIGHTER_STATE_KICK_LOW:
          switch (state_counter) {
            case 1:
              frame = FRAME_LOW_KICK_1;
              break;
            case 2:
              frame = FRAME_LOW_KICK_2;
              break;
            case 3:
              frame = FRAME_LOW_KICK_1;
              break;
            default:
              ChangeState(FIGHTER_STATE_CROUCH);
          }
          break;
        case FIGHTER_STATE_KICK_MID:
          switch (state_counter) {
            case 1:
              frame = FRAME_MID_KICK_1;
              break;
            case 2:
              frame = FRAME_MID_KICK_2;
              break;
            case 3:
              frame = FRAME_MID_KICK_3;
              break;
            case 4:
              frame = FRAME_MID_KICK_2;
              break;
            case 5:
              frame = FRAME_MID_KICK_1;
              break;
            default:
              ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_PUNCH_MID:
          switch (state_counter) {
            case 1:
              frame = FRAME_MID_PUNCH_1;
              break;
            case 2:
              frame = FRAME_MID_PUNCH_2;
              break;
            case 3:
              frame = FRAME_MID_PUNCH_1;
              break;
            default:
              ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_STAND:
          frame = FRAME_STAND;
          if (y < scene_floor) {
            y += 2;
          } else {
            y = scene_floor;
          }
          break;
        case FIGHTER_STATE_WALK:
          if (target.x < x - safe_space) {
            dir = FACE_LEFT;
            x_dir = -3;
          } else {
            if (target.x > x + safe_space) {
              dir = FACE_RIGHT;
              x_dir = 3;
            } else {
              x_dir = 0;
              ChangeState(FIGHTER_STATE_STAND);
            }
          }

          frame = frame == FRAME_STAND ? FRAME_WALK : FRAME_STAND;
          x += x_dir;
          break;
      }
    }

    void Update(float force_x, float force_y, byte c, byte z, byte csticky, byte zsticky)
    {
      state_counter++;
      switch (state) {
        case FIGHTER_STATE_BLOCK_MID:
          frame = FRAME_BLOCK;
          if (!csticky || !zsticky) {
            ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_BLOCK_LOW:
          frame = FRAME_LOW_BLOCK;
          if (!csticky || !zsticky) {
            ChangeState(FIGHTER_STATE_CROUCH);
          }
          break;
        case FIGHTER_STATE_HIT:
          switch (state_counter) {
            case 1:
              frame = FRAME_DEATH_1;
              break;
            case 2:
              frame = FRAME_DEATH_2;
              break;
          }
          x += x_dir;
          if (y < scene_floor) {
            y += 2;
          } else {
            y = scene_floor;
          }
          if (state_counter > 6) {
            x_dir = 0;
            ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_JUMP:
        case FIGHTER_STATE_KICK_HIGH:
          switch (state_counter) {
            case 1:
              frame = FRAME_HIGH_KICK_1;
              break;
            default:
              frame = FRAME_HIGH_KICK_1;
          }
          if (state_counter < 5) {
            if (z) { // don't reset the state counter
              state = FIGHTER_STATE_KICK_HIGH;
              frame = FRAME_HIGH_KICK_2;
            }
          }
          if (state_counter <= 5) {
            y -= 2;
          } else {
            if (y < scene_floor) {
              y += 2;
            } else {
              y = scene_floor;
            }
          }
          if (state_counter >= 10) {
            x_dir = 0;
            ChangeState(FIGHTER_STATE_STAND);
          }
          x += x_dir;
          break;

        case FIGHTER_STATE_CROUCH:
          frame = FRAME_CROUCH;
          if (force_y > -0.125) {
            ChangeState(FIGHTER_STATE_STAND);
          }
          if (csticky && zsticky) {
            ChangeState(FIGHTER_STATE_BLOCK_LOW);
          } else {
            if (c) {
              ChangeState(FIGHTER_STATE_PUNCH_LOW);
            }
            if (z) {
              ChangeState(FIGHTER_STATE_KICK_LOW);
            }
          }
          break;
        case FIGHTER_STATE_PUNCH_LOW:
          switch (state_counter) {
            case 1:
              frame = FRAME_LOW_BLOCK;
              break;
            case 2:
              frame = FRAME_LOW_PUNCH;
              break;
            case 3:
              frame = FRAME_LOW_BLOCK;
              break;
            default:
              ChangeState(FIGHTER_STATE_CROUCH);
          }
          break;
        case FIGHTER_STATE_KICK_LOW:
          switch (state_counter) {
            case 1:
              frame = FRAME_LOW_KICK_1;
              break;
            case 2:
              frame = FRAME_LOW_KICK_2;
              break;
            case 3:
              frame = FRAME_LOW_KICK_1;
              break;
            default:
              ChangeState(FIGHTER_STATE_CROUCH);
          }
          break;
        case FIGHTER_STATE_KICK_MID:
          switch (state_counter) {
            case 1:
              frame = FRAME_MID_KICK_1;
              break;
            case 2:
              frame = FRAME_MID_KICK_2;
              break;
            case 3:
              frame = FRAME_MID_KICK_3;
              break;
            case 4:
              frame = FRAME_MID_KICK_2;
              break;
            case 5:
              frame = FRAME_MID_KICK_1;
              break;
            default:
              ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_PUNCH_MID:
          switch (state_counter) {
            case 1:
              frame = FRAME_MID_PUNCH_1;
              break;
            case 2:
              frame = FRAME_MID_PUNCH_2;
              break;
            case 3:
              frame = FRAME_MID_PUNCH_1;
              break;
            default:
              ChangeState(FIGHTER_STATE_STAND);
          }
          break;
        case FIGHTER_STATE_STAND:
          if (force_x > 0.125 || force_x < -0.125) {
            ChangeState(FIGHTER_STATE_WALK);
          }
          if (force_y < -0.125) {
            ChangeState(FIGHTER_STATE_CROUCH);
          }
          if (force_y > 0.125) {
            ChangeState(FIGHTER_STATE_JUMP);
          }
          if (csticky && zsticky) {
            ChangeState(FIGHTER_STATE_BLOCK_MID);
          } else {
            if (c) {
              ChangeState(FIGHTER_STATE_PUNCH_MID);
            }
            if (z) {
              ChangeState(FIGHTER_STATE_KICK_MID);
            }
          }
          frame = FRAME_STAND;
          if (y < scene_floor) {
            y += 2;
          } else {
            y = scene_floor;
          }
          break;
        case FIGHTER_STATE_WALK:
          if (force_x > 0.125) {
            x_dir = 3;
            dir = FACE_RIGHT;
          }
          if (force_x < -0.125) {
            x_dir = -3;
            dir = FACE_LEFT;
          }
          if (force_x < 0.125 && force_x > -0.125) {
            x_dir = 0;
            ChangeState(FIGHTER_STATE_STAND);
          }
          if (force_y > 0.125) {
            ChangeState(FIGHTER_STATE_JUMP);
          }
          frame = frame == FRAME_STAND ? FRAME_WALK : FRAME_STAND;
          x += x_dir;
          break;
      }
    }

    void Render()
    {
      SpriteRender::RenderFrame(x, y - 40, dir, frame);
    }

    static void HitCheck(Fighter &A, Fighter &B)
    {
      if (abs(A.x - B.x) > 20) {
        return;
      }
      byte short_range = abs(A.x - B.x) < 12 ? 1 : 0;
      byte points = short_range ? 2 : 3;

      if (A.state == FIGHTER_STATE_HIT) {
        return;
      }

      if (B.state == FIGHTER_STATE_HIT) {
        return;
      }

      if (A.state ==  FIGHTER_STATE_BLOCK_LOW) {
        return;
      }
      if (A.state ==  FIGHTER_STATE_BLOCK_MID) {
        return;
      }
      if (A.state == FIGHTER_STATE_CROUCH) {
        return;
      }
      if (A.state == FIGHTER_STATE_JUMP) {
        return;
      }

      if (A.x > B.x && A.dir == FACE_RIGHT) {
        return;
      }

      if (A.x < B.x && A.dir == FACE_LEFT) {
        return;
      }


      switch (A.state)
      {
        case FIGHTER_STATE_STAND:
        case FIGHTER_STATE_WALK:
          return;

        case FIGHTER_STATE_PUNCH_MID:
          if (short_range == 0) {
            return;
          }
          if (B.state == FIGHTER_STATE_BLOCK_MID) {
            return;
          }
          break;

        case FIGHTER_STATE_KICK_MID:
          if (B.state == FIGHTER_STATE_BLOCK_MID) {
            return;
          }
          break;

        case FIGHTER_STATE_PUNCH_LOW:
          if (short_range == 0) {
            return;
          }
          if (B.state == FIGHTER_STATE_BLOCK_LOW) {
            return;
          }
          break;

        case FIGHTER_STATE_KICK_LOW:
          if (B.state == FIGHTER_STATE_BLOCK_LOW) {
            return;
          }
          break;

        case FIGHTER_STATE_KICK_HIGH:
          if (B.state == FIGHTER_STATE_CROUCH) {
            return;
          }
          break;
      }

      if (B.x < A.x) {
        B.dir = FACE_RIGHT;
        B.x_dir = -4;
      } else {
        B.dir = FACE_LEFT;
        B.x_dir = 4;
      }
      B.ChangeState(FIGHTER_STATE_HIT);
      B.health -= points;
      B.is_aggressive = 0;
      if (B.health < 0 || B.health > 100) {
        B.health = 0;
      }

    }
};
