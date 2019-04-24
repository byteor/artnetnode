#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>

#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif

// TODO: 1. handle button to reset configuration

#include "board.h"

Board board;

void setup()
{
    board.start();

    // by the time when we got here, WiFi is already connected
    delay(200);

    //ESP.eraseConfig(); // TODO: comment it out!
}

void loop()
{
    board.handle();
}
