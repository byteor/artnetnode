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

#include "UdpHandler.h"
#include "strobe.h"
#include "board.h"

UdpHandler udpHandler(&Serial);

int LED_PIN = 5;
Strobe strobe(LED_PIN);

#define UDP_PORT 6454 // ArtNet UDP port

Board board;

//callback notifying us of the need to save config
void saveConfigCallback(AsyncWebServerRequest *request)
{
    Serial.println("saving config");

/*
    if (request->hasParam("channel", true))
    {
        dmxChannel = request->getParam("channel", true)->value().toInt();
        Serial.print("DMX channel:");
        Serial.println(dmxChannel);
    }

    File file = SPIFFS.open("/config/config.json", "w");
    if (!file)
    {
        Serial.println("cannot save config file");
    }
    else
    {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, file);
        doc["dmxChannel"] = dmxChannel;
        serializeJson(doc, file);
        file.close();
        Serial.println("config file saved");
    }
    */
}

void setup()
{
    board.setSaveConfigCallback(saveConfigCallback);
    board.start();

    // by the time when we got here, WiFi is already connected
    delay(200);

    //ESP.eraseConfig(); // TODO: comment it out!

    /*
    if (!udpHandler.start(UDP_PORT, [](int channel, int value) {
            Serial.print("[");
            Serial.print(channel);
            Serial.print("]=");
            Serial.print(value);
            Serial.println();
            // data array is zero-based when DMX channels start with "1"
            if (channel == dmxChannel - 1)
            {
                if (value >= 127)
                {
                    Serial.println("ON");
                    strobe.start();
                }
                else
                {
                    Serial.println("OFF");
                    strobe.stop();
                }
            }
            if (channel == dmxChannel)
            {
                Serial.println(value * 5); // TODO: move this multiplier to the config
                strobe.setInterval(value * 5);
            }
        }))
    {
        Serial.print("couldn't start UDP");
    }
    */
}

void loop()
{
    board.handle();

    //strobe.handle();
}
