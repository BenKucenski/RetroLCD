// https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/

class RotaryDial {

  public:
    int rotaryCounter;
    char rotaryDirection;
    byte rotaryClock;
    byte rotaryLastClock;
    byte rotarySwitch;
    byte rotarySwitchMomentary;
    byte rotarySwitchLocked;

    byte ClockPin, DataPin, SwitchPin;

    void Init(byte pClockPin, byte pDataPin, byte pSwitchPin)
    {
      ClockPin = pClockPin;
      DataPin = pDataPin;
      SwitchPin = pSwitchPin;

      pinMode(ClockPin, INPUT);
      pinMode(DataPin, INPUT);
      pinMode(SwitchPin, INPUT);

      rotaryCounter = 0;

      rotaryLastClock = digitalRead(ClockPin);

      // NOTE: This function call belong in Init in your main program file.  It cannot be done in this class
      // you must use Digital Pin 2 or 3 for this - see following link if it's not working
      // you may be using one of the few boards were pin 2 or 3 isn't an option
      // note: the nano can only use D2 and D3 so only 2 rotary encoders are supported
      // https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
      //attachInterrupt(digitalPinToInterrupt(Player1Dial.ClockPin), ReadRotaryDial, CHANGE);
    }

    void ReadSwitch()
    {
      // read the button
      rotarySwitch = !digitalRead(SwitchPin);
      if (rotarySwitch == 1) {
        if (!rotarySwitchLocked) {
          rotarySwitchMomentary = 1;
        }
        else {
          rotarySwitchMomentary = 0;
        }
        rotarySwitchLocked = 1;
      } else {
        rotarySwitchLocked = 0;
        rotarySwitchMomentary = 0;
      }
    }

    void ReadRotaryDial()
    {
      // read the dial
      rotaryClock = digitalRead(ClockPin);

      if (rotaryClock != rotaryLastClock) {
        if (digitalRead(DataPin) != rotaryClock) {
          rotaryDirection = 1;
          rotaryCounter++;
        } else {
          rotaryDirection = 0;
          rotaryCounter--;
        }
      }
      rotaryLastClock = rotaryClock;
    }
};
