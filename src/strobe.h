#include <Arduino.h>

// default strobe pulse length, ms
#define DEFAULT_STROBE_PULSE 20

class Strobe 
{
    protected:
    int pin;
    int pulse;  // 'active' duration, ms
    int period; // 'total' duration, ms
    bool enabled;
    int activeState;
    int inactiveState;
    int state;

    unsigned long previousMillis = 0;
    unsigned long interval;

    public:
    Strobe(int pin = LED_BUILTIN, int pulse = DEFAULT_STROBE_PULSE, int activeState = HIGH);
    void start();
    void stop();
    void setInterval(int millis);
    void handle();
};