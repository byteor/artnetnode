#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "strobe.h"
#include "config.h"

/*
#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif
*/
typedef std::function<void(AsyncWebServerRequest *request)> SaveConfigCallback;

class Board
{
    Config conf;

protected:
    const uint16_t WWW_PORT = 80;
    AsyncWebServer *server = new AsyncWebServer(WWW_PORT);
    DNSServer dns;
    // DNS server
    const byte DNS_PORT = 53;

    const char *AP_PASS = "secret"; // Access point mode password

    AsyncEventSource *events = new AsyncEventSource("/events");

    SaveConfigCallback saveConfigCallback;

    bool conectWiFi(String ssid, String pass, String host);
    bool isIp(String str);
    String toStringIp(IPAddress ip);
    void startCaptivePortal();
    void setupConfigPages();

public:
    Board();

    void printWifiStatus();

    String getHostName();

    void setSaveConfigCallback(SaveConfigCallback callback);

    void handle();
    void init();
    void start();
};