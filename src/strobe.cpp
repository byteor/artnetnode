#include "strobe.h"

Strobe::Strobe(int pin, int pulse, int activeState)
{
    this->pin = pin;
    this->pulse = pulse;
    enabled = false;
    pinMode(pin, OUTPUT);
    this->activeState = activeState;
    this->inactiveState = activeState == HIGH ? LOW : HIGH;
    interval = 0;
}

void Strobe::setInterval(int millis)
{
    period = millis;
    if(period < 0) period = pulse;
}

void Strobe::handle()
{
    unsigned long currentMillis = millis();
    if (period <= pulse && enabled)
    {
        previousMillis = currentMillis;
        if(state != activeState)
        {
            state = activeState;
            digitalWrite(pin, state);
        }
    }
    else
    {
        if (currentMillis - previousMillis >= interval && enabled)
        {
            previousMillis = currentMillis;
            if (state == activeState)
            {
                state = inactiveState;
                interval = period - pulse;
            }
            else
            {
                state = activeState;
                interval = pulse;
            }
            digitalWrite(pin, state);
        }
    }
}

void Strobe::start()
{
    if(!enabled)
    {
        interval = 0;
        state = inactiveState;
        enabled = true;
    }
}

void Strobe::stop()
{
    enabled = false;
    digitalWrite(pin, inactiveState);
}