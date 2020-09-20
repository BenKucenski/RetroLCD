/**
 * @license Nunchuk Arduino library v0.0.1 16/12/2016
 * http://www.xarg.org/2016/12/arduino-nunchuk-library/
 *
 * Copyright (c) 2016, Robert Eisele (robert@xarg.org)
 * Dual licensed under the MIT or GPL Version 2 licenses.
 * Refactor: bkucenski@gmail.com 2/22/2019
 **/

#ifndef NUNCHUK_H
#define NUNCHUK_H

#include <Wire.h>

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


uint8_t nunchuk_data[6];
uint8_t nunchuk_cali[16];

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) // Only Arduino UNO
/**
 * Use normal analog ports as power supply, which is useful if you want to have all pins in a row
 * Like for the famous WiiChuck adapter
 * @see https://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 */
static void NunchukInitPower() {
    // Add power supply for port C2 (GND) and C3 (PWR)
    PORTC &= ~_BV(PORTC2);
    PORTC |= _BV(PORTC3);
    DDRC |= _BV(PORTC2) | _BV(PORTC3);
    delay(100);
}
#endif
/**
 * Initializes the Nunchuk communication by sending a sequence of bytes
 */
static void NunchukInit() {

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
 * Central function to read a full chunk of data from Nunchuk
 *
 * @return A boolean if the data transfer was successful
 */
static uint8_t NunchukRead() {

    uint8_t i;
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
 * Checks the current state of button Z
 */
static uint8_t NunchukZ() {
    return (~nunchuk_data[5] >> 0) & 1;
}

/**
 * Checks the current state of button C
 */
static uint8_t NunchukC() {
    return (~nunchuk_data[5] >> 1) & 1;
}

/**
 * Retrieves the raw X-value of the joystick
 */
static uint8_t NunchukXRaw() {
    return nunchuk_data[0];
}

/**
 * Retrieves the raw Y-value of the joystick
 */
static uint8_t NunchukYRaw() {
    return nunchuk_data[1];
}

/**
 * Retrieves the calibrated X-value of the joystick
 */
static int16_t NunchukX() {
    return (int16_t) nunchuk_data[0] - (int16_t) NUNCHUK_JOYSTICK_X_ZERO;
}

/**
 * Retrieves the calibrated Y-value of the joystick
 */
static int16_t NunchukY() {
    return (int16_t) nunchuk_data[1] - (int16_t) NUNCHUK_JOYSTICK_Y_ZERO;
}

/**
 * Calculates the angle of the joystick
 */
static float NunchukAngle() {
    return atan2((float) NunchukX(), (float) NunchukY());
}

/**
 * Retrieves the raw X-value of the accelerometer
 */
static uint16_t NunchukXAccelRaw() {
    return ((uint16_t) nunchuk_data[2] << 2) | ((nunchuk_data[5] >> 2) & 3);
}

/**
 * Retrieves the raw Y-value of the accelerometer
 */
static uint16_t NunchukYAccelRaw() {
    return ((uint16_t) nunchuk_data[3] << 2) | ((nunchuk_data[5] >> 4) & 3);
}

/**
 * Retrieves the raw Z-value of the accelerometer
 */
static uint16_t NunchukZAccelRaw() {
    return ((uint16_t) nunchuk_data[4] << 2) | ((nunchuk_data[5] >> 6) & 3);
}

/**
 * Retrieves the calibrated X-value of the accelerometer
 */
static int16_t NunchukXAccel() {
    return (int16_t) NunchukXAccelRaw() - (int16_t) NUNCHUK_ACCEL_X_ZERO;
}

/**
 * Retrieves the calibrated Y-value of the accelerometer
 */
static int16_t NunchukYAccel() {
    return (int16_t) NunchukYAccelRaw() - (int16_t) NUNCHUK_ACCEL_Y_ZERO;
}

/**
 * Retrieves the calibrated Z-value of the accelerometer
 */
static int16_t NunchukZAccel() {
    return (int16_t) NunchukZAccelRaw() - (int16_t) NUNCHUK_ACCEL_Z_ZERO;
}

/**
 * Calculates the pitch angle THETA around y-axis of the Nunchuk in radians
 */
static float NunchukPitch() { // tilt-y
    return atan2((float) NunchukYAccel(), (float) NunchukZAccel());
}

/**
 * Calculates the roll angle PHI around x-axis of the Nunchuk in radians
 */
static float NunchukRoll() { // tilt-x
    return atan2((float) NunchukXAccel(), (float) NunchukZAccel());
}

/**
 * A handy function to print either verbose information of the Nunchuk or a CSV stream for Processing
 */
static void NunchukPrintDebug() {

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

#endif
