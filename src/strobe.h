#ifndef STROBE_H
#define STROBE_H

#include <Arduino.h>

// default strobe pulse length, ms
#define DEFAULT_STROBE_PULSE 5

class Strobe
{
    protected:
    uint8_t pin;
    int pulse;  // 'active' duration, ms
    int period; // 'total' duration, ms
    bool enabled;
    uint8_t activeState;
    uint8_t inactiveState;
    bool dimmable;      // when Dimmable 'value' is being used, 'state' otherwise
    int state;          // 1/0 == ON/OFF
    int value;          // original PWM value, 0-255
    int adjustedActiveValue;  // PWM value adjusted to the 'active' level
    int adjustedInactiveValue;  // PWM value adjusted to the 'active' level

    unsigned long previousMillis = 0;
    unsigned long interval;

    public:
    Strobe(uint8_t pin = LED_BUILTIN, int pulse = DEFAULT_STROBE_PULSE, int activeState = HIGH, bool dimmable = false);
    void start();
    void start(uint8_t value);
    void stop();
    void setInterval(int millis);
    void setDuration(int millis);
    void setPin(int number);
    void handle();
};

#endif