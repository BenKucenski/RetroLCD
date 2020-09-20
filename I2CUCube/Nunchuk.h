/**
   @license Nunchuk Arduino library v0.0.1 16/12/2016
   http://www.xarg.org/2016/12/arduino-nunchuk-library/

   Copyright (c) 2016, Robert Eisele (robert@xarg.org)
   Dual licensed under the MIT or GPL Version 2 licenses.
   Refactor: bkucenski@gmail.com 2/22/2019
 **/

#ifndef NUNCHUK_H
#define NUNCHUK_H

#define byte uint8_t

// The Nunchuk I2C address
#define NUNCHUK_ADDRESS 0x52

// the clock speed, 400,000 works
#define CLOCK_NUNCHUK 400000

// Calibration accelerometer values, depends on your Nunchuk
#define NUNCHUK_ACCEL_X_ZERO 512
#define NUNCHUK_ACCEL_Y_ZERO 512
#define NUNCHUK_ACCEL_Z_ZERO 512

// Calibration joystick values
#define NUNCHUK_JOYSTICK_X_ZERO 127
#define NUNCHUK_JOYSTICK_Y_ZERO 128

class Nunchuk
{
  private:
    static byte nunchuk_data[6];
    static byte nunchuk_cali[16];

    static byte nunchuk_buttonset[2];

  public:
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) // Only Arduino UNO
    /**
       this is only for the Arduino UNO
       Use normal analog ports as power supply, which is useful if you want to have all pins in a row
       Like for the famous WiiChuck adapter
       @see https://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
    */
    static void NunchukInitPower();
#endif
    /**
       Initializes the Nunchuk communication by sending a sequence of bytes
    */
    static void NunchukInit();

    /**
       Central function to read a full chunk of data from Nunchuk

       @return A boolean if the data transfer was successful
    */
    static byte NunchukRead();

    /**
       Checks the current state of button Z
    */
    static byte NunchukZ();

    static byte NunchukZAlt();

    /**
       Checks the current state of button C
    */
    static byte NunchukC();

    static byte NunchukCAlt();

    /**
       Retrieves the raw X-value of the joystick
    */
    static byte NunchukXRaw();

    /**
       Retrieves the raw Y-value of the joystick
    */
    static byte NunchukYRaw();

    /**
       Retrieves the calibrated X-value of the joystick
    */
    static int NunchukX();

    /**
       Retrieves the calibrated Y-value of the joystick
    */
    static int NunchukY();

    /**
       Calculates the angle of the joystick
    */
    static float NunchukAngle();

    /**
       Retrieves the raw X-value of the accelerometer
    */
    static word  NunchukXAccelRaw();

    /**
       Retrieves the raw Y-value of the accelerometer
    */
    static word  NunchukYAccelRaw();

    /**
       Retrieves the raw Z-value of the accelerometer
    */
    static word  NunchukZAccelRaw();

    /**
       Retrieves the calibrated X-value of the accelerometer
    */
    static int NunchukXAccel();

    /**
       Retrieves the calibrated Y-value of the accelerometer
    */
    static int NunchukYAccel();

    /**
       Retrieves the calibrated Z-value of the accelerometer
    */
    static int NunchukZAccel();

    /**
       Calculates the pitch angle THETA around y-axis of the Nunchuk in radians
    */
    static float NunchukPitch();

    /**
       Calculates the roll angle PHI around x-axis of the Nunchuk in radians
    */
    static float NunchukRoll();

    /**
       A handy function to print either verbose information of the Nunchuk or a CSV stream for Processing
    */
    static void NunchukPrintDebug();
};






#endif
