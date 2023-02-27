class Light
{
private:
    bool _state;
    unsigned char _pin;
    char _manual;
    unsigned long _timer;
public:
    Light(unsigned char pin);
    void begin();
    void turnOn();
    void turnOnWithTimer();
    void turnOff();
    bool getState();
    void turnOnManual();
    void turnOffManual();
    bool isManual();
    bool isOn();
    void manageLightState();
};