/**
   @license Nunchuk Arduino library v0.0.1 16/12/2016
   http://www.xarg.org/2016/12/arduino-nunchuk-library/

   Copyright (c) 2016, Robert Eisele (robert@xarg.org)
   Dual licensed under the MIT or GPL Version 2 licenses.
   Refactor: bkucenski@gmail.com 2/22/2019
 **/

#include <Wire.h>
#include "Arduino.h"
#include "Nunchuk.h"

byte Nunchuk::nunchuk_data[6];
byte Nunchuk::nunchuk_cali[16];

static byte Nunchuk::nunchuk_buttonset[2];

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) // Only Arduino UNO
/**
   this is only for the Arduino UNO
   Use normal analog ports as power supply, which is useful if you want to have all pins in a row
   Like for the famous WiiChuck adapter
   @see https://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
*/
static void Nunchuk::NunchukInitPower() {
  // Add power supply for port C2 (GND) and C3 (PWR)
  PORTC &= ~_BV(PORTC2);
  PORTC |= _BV(PORTC3);
  DDRC |= _BV(PORTC2) | _BV(PORTC3);
  //delay(100);
}
#endif
/**
   Initializes the Nunchuk communication by sending a sequence of bytes
*/
static void Nunchuk::NunchukInit() {

  nunchuk_buttonset[0] = 0;
  nunchuk_buttonset[1] = 0;

  // Change TWI speed for nuchuk, which uses Fast-TWI (400kHz)
  Wire.setClock(CLOCK_NUNCHUK);

  // delay(1);

  Wire.beginTransmission(NUNCHUK_ADDRESS);
  Wire.write(0xF0);
  Wire.write(0x55);
  Wire.endTransmission();
  // delay(1);
  Wire.beginTransmission(NUNCHUK_ADDRESS);
  Wire.write(0xFB);
  Wire.write(0x00);
  Wire.endTransmission();
  // delay(1);


}

/**
   Central function to read a full chunk of data from Nunchuk

   @return A boolean if the data transfer was successful
*/
static byte Nunchuk::NunchukRead() {

  byte i;
  Wire.requestFrom(NUNCHUK_ADDRESS, 6);
  //delayMicroseconds(10);
  for (i = 0; i < 6 && Wire.available(); i++) {
    nunchuk_data[i] = Wire.read();
  }
  Wire.beginTransmission(NUNCHUK_ADDRESS);
  Wire.write(0x00);
  // delayMicroseconds(100);
  Wire.endTransmission();
  return i == 6;
}

/**
   Checks the current state of button Z
*/
static byte Nunchuk::NunchukZ() {
  return (~nunchuk_data[5] >> 0) & 1;
}

static byte Nunchuk::NunchukZAlt() {
  byte b = (~nunchuk_data[5] >> 0) & 1;
  if (b == 1 && nunchuk_buttonset[0] == 1) {
    return 0;
  }
  nunchuk_buttonset[0] = b;
  return b;
}

/**
   Checks the current state of button C
*/
static byte Nunchuk::NunchukC() {
  return (~nunchuk_data[5] >> 1) & 1;
}

static byte Nunchuk::NunchukCAlt() {
  byte b = (~nunchuk_data[5] >> 1) & 1;
  if (b == 1 && nunchuk_buttonset[1] == 1) {
    return 0;
  }
  nunchuk_buttonset[1] = b;
  return b;
}

/**
   Retrieves the raw X-value of the joystick
*/
static byte Nunchuk::NunchukXRaw() {
  return nunchuk_data[0];
}

/**
   Retrieves the raw Y-value of the joystick
*/
static byte Nunchuk::NunchukYRaw() {
  return nunchuk_data[1];
}

/**
   Retrieves the calibrated X-value of the joystick
*/
static int Nunchuk::NunchukX() {
  return (int) nunchuk_data[0] - (int) NUNCHUK_JOYSTICK_X_ZERO;
}

/**
   Retrieves the calibrated Y-value of the joystick
*/
static int Nunchuk::NunchukY() {
  return (int) nunchuk_data[1] - (int) NUNCHUK_JOYSTICK_Y_ZERO;
}

/**
   Calculates the angle of the joystick
*/
static float Nunchuk::NunchukAngle() {
  return atan2((float) NunchukX(), (float) NunchukY());
}

/**
   Retrieves the raw X-value of the accelerometer
*/
static word Nunchuk::NunchukXAccelRaw() {
  return ((word) nunchuk_data[2] << 2) | ((nunchuk_data[5] >> 2) & 3);
}

/**
   Retrieves the raw Y-value of the accelerometer
*/
static word Nunchuk::NunchukYAccelRaw() {
  return ((word) nunchuk_data[3] << 2) | ((nunchuk_data[5] >> 4) & 3);
}

/**
   Retrieves the raw Z-value of the accelerometer
*/
static word Nunchuk::NunchukZAccelRaw() {
  return ((word ) nunchuk_data[4] << 2) | ((nunchuk_data[5] >> 6) & 3);
}

/**
   Retrieves the calibrated X-value of the accelerometer
*/
static int  Nunchuk::NunchukXAccel() {
  return (int ) NunchukXAccelRaw() - (int ) NUNCHUK_ACCEL_X_ZERO;
}

/**
   Retrieves the calibrated Y-value of the accelerometer
*/
static int  Nunchuk::NunchukYAccel() {
  return (int ) NunchukYAccelRaw() - (int ) NUNCHUK_ACCEL_Y_ZERO;
}

/**
   Retrieves the calibrated Z-value of the accelerometer
*/
static int  Nunchuk::NunchukZAccel() {
  return (int ) NunchukZAccelRaw() - (int ) NUNCHUK_ACCEL_Z_ZERO;
}

/**
   Calculates the pitch angle THETA around y-axis of the Nunchuk in radians
*/
static float Nunchuk::NunchukPitch() { // tilt-y
  return atan2((float) NunchukYAccel(), (float) NunchukZAccel());
}

/**
   Calculates the roll angle PHI around x-axis of the Nunchuk in radians
*/
static float Nunchuk::NunchukRoll() { // tilt-x
  return atan2((float) NunchukXAccel(), (float) NunchukZAccel());
}

/**
   A handy function to print either verbose information of the Nunchuk or a CSV stream for Processing
*/
static void Nunchuk::NunchukPrintDebug() {

  Serial.print("joy: ");
  Serial.print(NunchukX(), DEC);
  Serial.print(", ");
  Serial.print(NunchukY(), DEC);

  Serial.print("  acc:");
  Serial.print(NunchukXAccel(), DEC);
  Serial.print(", ");
  Serial.print(NunchukYAccel(), DEC);
  Serial.print(", ");
  Serial.print(NunchukZAccel(), DEC);

  Serial.print("  but:");
  Serial.print(NunchukZ(), DEC);
  Serial.print(", ");
  Serial.print(NunchukC(), DEC);
  Serial.print("\n");
}
