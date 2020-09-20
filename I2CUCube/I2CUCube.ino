#include <Arduino.h>
#include <Wire.h>
#include "Nunchuk.h"
#include "Display128x64.h"
#include "LCD1602.h"
#include "Cube.h"

struct vect3d {
  float x, y, z;
};

struct triangle {
  vect3d a, b, c;
};

struct mat4x4 {
  float m[4][4] = { 0 };
};

// https://www.youtube.com/watch?v=ih20l3pJoeU
float aspect_ratio = 64.0 / 128.0; // screen height / width
float field_of_view = 90.0 * 3.14 / 180.0; // 90 degrees converted to radians

float zfar = 1000.0;
float znear = 0.1;

float xy_proj = 1.0 / tan(field_of_view / 2.0);
float z_proj_a = zfar / (zfar - znear);
float z_proj_b = z_proj_a * znear;

mat4x4 matProj;
vect3d vCamera;


class Math3d {
  private:

  public:
    static void MultiplyMatrixVector(vect3d &i, vect3d &o, mat4x4 &m)
    {
      o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
      o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
      o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
      float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
      if (w != 0) {
        o.x /= w;
        o.y /= w;
        o.z /= w;
      }
    }

    static void Difference(vect3d &a, vect3d &b, vect3d &o) {
      o.x = b.x - a.x;
      o.y = b.y - a.y;
      o.z = b.z - a.z;
    }

    static void CrossProduct(vect3d &a, vect3d &b, vect3d &o) {
      o.x = a.y * b.z - a.z * b.y;
      o.y = a.z * b.x - a.x * b.z;
      o.z = a.x * b.y - a.y * b.x;
    }

    static float DotProduct(vect3d &a, vect3d &b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static float DotProductTranslate(vect3d &a, vect3d &b, vect3d &t) {
      return
        a.x * (b.x - t.x)
        + a.y * (b.y - t.y)
        + a.z * (b.z - t.z);
    }

    static float Distance(vect3d &a, vect3d &b) {
      return sqrtf(
               (a.x - b.x) * (a.x - b.x)
               + (a.y - b.y) * (a.y - b.y)
               + (a.z - b.z) * (a.z - b.z)
             );
    }

    static void Normalize(vect3d &a) {
      float d = 1.0 / sqrtf(
                  a.x * a.x
                  + a.y * a.y
                  + a.z * a.z
                );
      a.x *= d;
      a.y *= d;
      a.z *= d;
    }


    static vect3d Normal(vect3d &a, vect3d &b, vect3d &o) {
      float d = 1.0 / Distance(a, b);
      o.x = (a.x - b.x) * d;
      o.y = (a.y - b.y) * d;
      o.z = (a.z - b.z) * d;
    }


};


byte CurrentFrame;

void setup() {
  Wire.begin();

  // Nunchuk::NunchukInit();
  Display128x64::InitDisplay(SSD1306_SWITCHCAPVCC);

  // Nunchuk::NunchukRead();
  // delay(100);
  randomSeed(analogRead(0));

  CurrentFrame = 0;

  matProj.m[0][0] = aspect_ratio * field_of_view;
  matProj.m[1][1] = field_of_view;
  matProj.m[2][2] = z_proj_a;
  matProj.m[3][3] = z_proj_b;
  matProj.m[2][3] = 1.0;
  matProj.m[3][3] = 0.0;
}

void TriangleFromVerticles(triangle &source, const float * vertices, int j) {
  source.a.x = pgm_read_float(vertices + 0 + j * 9);
  source.a.y = pgm_read_float(vertices + 1 + j * 9);
  source.a.z = pgm_read_float(vertices + 2 + j * 9);

  source.b.x = pgm_read_float(vertices + 3 + j * 9);
  source.b.y = pgm_read_float(vertices + 4 + j * 9);
  source.b.z = pgm_read_float(vertices + 5 + j * 9);

  source.c.x = pgm_read_float(vertices + 6 + j * 9);
  source.c.y = pgm_read_float(vertices + 7 + j * 9);
  source.c.z = pgm_read_float(vertices + 8 + j * 9);
}

void DrawCube(float elapsedTime)
{
  int j;
  mat4x4 matRotZ, matRotX;

  float fTheta = elapsedTime * 4.0 * 3.14;

  // rotation z
  matRotZ.m[0][0] = cosf(fTheta);
  matRotZ.m[0][1] = sinf(fTheta);
  matRotZ.m[1][0] = -sinf(fTheta);
  matRotZ.m[1][1] = cosf(fTheta);
  matRotZ.m[2][2] = 1;
  matRotZ.m[3][3] = 1;

  // rotation x
  matRotX.m[0][0] = 1;
  matRotX.m[1][1] = cosf(fTheta * 0.5);
  matRotX.m[1][2] = sinf(fTheta * 0.5);
  matRotX.m[2][1] = -sinf(fTheta * 0.5);
  matRotX.m[3][2] = cosf(fTheta * 0.5);
  matRotX.m[3][3] = 1;

  for (j = 0; j < 12; j++) {
    triangle source, triProjected, triTranslated, triRotatedZ, triRotatedZX;

    TriangleFromVerticles(source, Object_Cube, j);
    

    // rotate the triangle in the Z-Axis
    Math3d::MultiplyMatrixVector(source.a, triRotatedZ.a, matRotZ);
    Math3d::MultiplyMatrixVector(source.b, triRotatedZ.b, matRotZ);
    Math3d::MultiplyMatrixVector(source.c, triRotatedZ.c, matRotZ);

    // rotate the triangle in the x-Axis
    Math3d::MultiplyMatrixVector(triRotatedZ.a, triRotatedZX.a, matRotX);
    Math3d::MultiplyMatrixVector(triRotatedZ.b, triRotatedZX.b, matRotX);
    Math3d::MultiplyMatrixVector(triRotatedZ.c, triRotatedZX.c, matRotX);

    // translate it
    triTranslated = triRotatedZX;
    triTranslated.a.z = triRotatedZX.a.z + 3.0f;
    triTranslated.b.z = triRotatedZX.b.z + 3.0f;
    triTranslated.c.z = triRotatedZX.c.z + 3.0f;

    //culling
    vect3d normal, line1, line2;
    Math3d::Difference(triTranslated.a, triTranslated.b, line1);
    Math3d::Difference(triTranslated.a, triTranslated.c, line2);
    Math3d::CrossProduct(line1, line2, normal);
    Math3d::Normalize(normal);

    //if(normal.z >= 0) {
    if (Math3d::DotProductTranslate(normal, triTranslated.a, vCamera) >= 0) {
      continue;
    }

    // end culling

    // project it to screen coordinates
    Math3d::MultiplyMatrixVector(triTranslated.a, triProjected.a, matProj);
    Math3d::MultiplyMatrixVector(triTranslated.b, triProjected.b, matProj);
    Math3d::MultiplyMatrixVector(triTranslated.c, triProjected.c, matProj);


    triProjected.a.x += 1.0;  triProjected.a.y += 1.0;
    triProjected.b.x += 1.0;  triProjected.b.y += 1.0;
    triProjected.c.x += 1.0;  triProjected.c.y += 1.0;

    triProjected.a.x *= 0.5 * 127.0;  triProjected.a.y *= 0.5 * 63.0;
    triProjected.b.x *= 0.5 * 127.0;  triProjected.b.y *= 0.5 * 63.0;
    triProjected.c.x *= 0.5 * 127.0;  triProjected.c.y *= 0.5 * 63.0;

    Display128x64::DrawTriangle(
      triProjected.a.x, triProjected.a.y,
      triProjected.b.x, triProjected.b.y,
      triProjected.c.x, triProjected.c.y,
      1);
  }

}

void RenderDisplay()
{
  Display128x64::ClearDisplay();

  DrawCube((float) CurrentFrame / 255.0);

  Wire.setClock(CLOCK_DISPLAY);
  Display128x64::PushToDisplay();
  CurrentFrame++;
}



void ReadController()
{
  Wire.setClock(CLOCK_NUNCHUK);

  if (!Nunchuk::NunchukRead()) {
    return;
  }

  float InputForceX;
  float InputForceY;

  InputForceX = map(Nunchuk::NunchukX(), -128, 127, 0, 255) * 1.0 / 128.0 - 1.0;
  InputForceY = (255 - map(Nunchuk::NunchukY(), 127, -128, 0, 255)) * 1.0 / 128.0 - 1.0;

}

void loop() {
  ReadController();

  RenderDisplay();


  CurrentFrame++;
  CurrentFrame %= 200;

  //delay(50); // each frame is 50 milliseconds
}
