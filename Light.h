class Light
{
private:
    bool _state;
    unsigned char _pin;
    String _mode;
public:
    Light(unsigned char pin);
    void begin();
    void turnOn();
    void turnOff();
    bool getState();
    void setMode(String mode);
    String getMode();
};