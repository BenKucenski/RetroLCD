
class Joystick {

  public:
    int XAxis;
    int YAxis;
    byte joySwitch;
    byte joySwitchMomentary;
    byte joySwitchLocked;

    byte XAxisPin, YAxisPin, SwitchPin;

    void Init(byte pXAxisPin, byte pYAxisPin, byte pSwitchPin)
    {
      XAxisPin = pXAxisPin;
      YAxisPin = pYAxisPin;
      SwitchPin = pSwitchPin;

      pinMode(SwitchPin, INPUT_PULLUP);
    }

    void ReadSwitch()
    {
      // read the button
      joySwitch = !digitalRead(SwitchPin);
      if (joySwitch == 1) {
        if (!joySwitchLocked) {
          joySwitchMomentary = 1;
        }
        else {
          joySwitchMomentary = 0;
        }
        joySwitchLocked = 1;
      } else {
        joySwitchLocked = 0;
        joySwitchMomentary = 0;
      }
    }

    void ReadJoystick()
    {
      XAxis = analogRead(XAxisPin);
      YAxis = analogRead(YAxisPin);

    }
};
