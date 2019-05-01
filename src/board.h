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
#include "UdpHandler.h"

/*
#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif
*/
#define UDP_PORT 6454 // ArtNet UDP port
#define WWW_PORT 80     // WWW port
#define DNS_PORT 53

#define CONFIG_FILE "config.json"

class Board
{
    Config conf;

protected:
    bool isRestartPending = false;
    AsyncWebServer *server = new AsyncWebServer(WWW_PORT);
    DNSServer dns;
    Strobe *strobes[MAX_DMX_CHANNELS];
    UdpHandler *udpHandler;

    const char *AP_PASS = "secret"; // Access point mode password

    AsyncEventSource *events = new AsyncEventSource("/events");

    bool conectWiFi(String ssid, String pass, String host);
    bool isIp(String str);
    String toStringIp(IPAddress ip);
    void startCaptivePortal();
    void setupConfigPages();

public:
    Board();

    void printWifiStatus();

    String getHostName();

    void handle();
    void init();
    void start();
    void restart();
};