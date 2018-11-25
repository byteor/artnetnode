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

#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif
#include <Servo.h>

#include "UdpHandler.h"
#include "strobe.h"

AsyncWebServer server(80);
DNSServer dns;
// DNS server
const byte DNS_PORT = 53;

AsyncEventSource events("/events");
UdpHandler udpHandler(&Serial);


int LED_PIN = 5; 
Strobe strobe(LED_PIN);

bool isIp(String str)
{
    for (int i = 0; i < str.length(); i++)
    {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9'))
        {
            return false;
        }
    }
    return true;
}
/** IP to String? */
String toStringIp(IPAddress ip)
{
    String res = "";
    for (int i = 0; i < 3; i++)
    {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}

void printWifiStatus();

// Parameters to wetip with a web server
char ssid[32];      // network SSID (name)
char pass[32];      // network password
char host[32];      // host name
long dmxChannel;    // DMX channel
const char *AP_PASS = "secret"; // Access point mode password
#define UDP_PORT 6454 // ArtNet UDP port

StaticJsonBuffer<4400> jsonBuffer;

//callback notifying us of the need to save config
void saveConfigCallback()
{
    Serial.println("saving config");
    File file = SPIFFS.open("/config/config.json", "w");
    if (!file)
    {
        Serial.println("cannot save config file");
    } 
    else
    {
        JsonObject &root = jsonBuffer.createObject();
        root["ssid"] = ssid;
        root["pass"] = pass;
        root["deviceName"] = host;
        root["dmxChannel"] = dmxChannel;
        root.printTo(file);
        file.close();
        Serial.println("config file saved");
    }
}

bool conectWiFi()
{
    Serial.print("\r\n[Connecting WiFi] ");
    Serial.print(ssid);
    Serial.print(" ");
    Serial.println(pass);
    WiFi.hostname(host);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        tries++;
        if (tries > 20)
        {
            Serial.println();
            return false;
        }
    }
    Serial.println();
    return true;
}

String processor(const String& var)
{
  if(var == "SSID")
    return ssid;
  if(var == "PASS")
    return pass;
  if(var == "HOST")
    return host;
  if(var == "CHANNEL")
    return String(dmxChannel);
  return String();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("");
    //Serial.setDebugOutput(true);
    SPIFFS.begin();
    bool configured = false;
    File file = SPIFFS.open("/config/config.json", "r");
    if (!file)
    {
        Serial.println("no config file");
    }
    else
    {
        size_t size = file.size();
        if (size == 0)
        {
            Serial.println("config file is empty");
        }
        else
        {
            std::unique_ptr<char[]> buf(new char[size]);
            file.readBytes(buf.get(), size);
            JsonObject &root = jsonBuffer.parseObject(buf.get());
            if (!root.success())
            {
                Serial.println("cannot parse config file");
            }
            else
            {
                Serial.println("config loaded");
                root.printTo(Serial);
                configured = true;
                strcpy(ssid, root["ssid"]);
                strcpy(pass, root["pass"]);
                strcpy(host, root["deviceName"]);
                dmxChannel = root["dmxChannel"];
            }
        }
        file.close();
    }

    //WiFiManager
    //ESP.eraseConfig(); // TODO: comment it out!

    bool gotNewConfig = false;
    delay(500);
    while (!conectWiFi())
    {
        dns.stop();
        server.reset();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(host, AP_PASS);
        delay(500);
        Serial.println(F("AP IP address: "));
        Serial.println(WiFi.softAPIP());
        // Setup the DNS server redirecting all the domains to the apIP
        dns.setErrorReplyCode(DNSReplyCode::NoError);
        dns.start(DNS_PORT, "*", WiFi.softAPIP());

        server.serveStatic("/", SPIFFS, "/ap/")/*.setDefaultFile("index.html").setFilter(ON_AP_FILTER)*/;
        server.serveStatic("/fs/", SPIFFS, "/fs/");
        server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/ap/index.html", String(), false, processor);
        });
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/ap/index.html", String(), false, processor);
        });
        server.on("/wifi", HTTP_POST, [&gotNewConfig](AsyncWebServerRequest *request) {
            //List all parameters
            /*
            int params = request->params();
            for (int i = 0; i < params; i++)
            {
                AsyncWebParameter *p = request->getParam(i);
                if (p->isFile())
                { //p->isPost() is also true
                    Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
                }
                else if (p->isPost())
                {
                    Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
                else
                {
                    Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
            }
            */
            if (request->hasParam("ssid", true) && request->hasParam("password", true))
            {
                AsyncWebParameter *pSsid = request->getParam("ssid", true);
                AsyncWebParameter *pPwd = request->getParam("password", true);
                long channel = 0;
                strcpy(ssid, pSsid->value().c_str());
                strcpy(pass, pPwd->value().c_str());
                if(request->hasParam("host", true))
                    strcpy(host, request->getParam("host", true)->value().c_str());
                if(request->hasParam("channel", true)) {
                    channel = request->getParam("channel", true)->value().toInt();
                }
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
                response->addHeader("Connection", "close");
                request->send(response);
                gotNewConfig = true;
                Serial.println("Got new config:");
                Serial.print("ssid:");
                Serial.println(ssid);
                Serial.print("pass:");
                Serial.println(pass);
                Serial.print("host:");
                Serial.println(host);
                Serial.print("channel:");
                Serial.println(channel);
                saveConfigCallback();
                return;
            }
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Location", String("http://") + toStringIp(request->client()->localIP()));
            request->send(response);
        });

        server.onNotFound([](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
            response->addHeader("Location", String("http://") + toStringIp(request->client()->localIP()));
            request->send(response);
        });
        server.begin();
        while (!gotNewConfig)
        {
            dns.processNextRequest();
        }
        gotNewConfig = false;
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    Serial.println();
    printWifiStatus();

    MDNS.addService("http", "tcp", 80);
    MDNS.begin(host);

    if (!udpHandler.start(UDP_PORT, [](int channel, int value) {
            Serial.print("[");
            Serial.print(channel);
            Serial.print("]=");
            Serial.print(value);
            Serial.println();
            // data array is zero-based when DMX channels start with "1"
            if(channel == dmxChannel - 1) {
                if(value >= 127) 
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
            if(channel == dmxChannel) {
                Serial.println(value * 5); // TODO: move this multiplier to the config
                strobe.setInterval(value * 5);
            }
        }))
    {
        Serial.print("couldn't start UDP");
    }

    // OTA
    //Send OTA events to the browser
    ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
    ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        char p[32];
        sprintf(p, "Progress: %u%%\n", (progress / (total / 100)));
        events.send(p, "ota");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        if (error == OTA_AUTH_ERROR)
            events.send("Auth Failed", "ota");
        else if (error == OTA_BEGIN_ERROR)
            events.send("Begin Failed", "ota");
        else if (error == OTA_CONNECT_ERROR)
            events.send("Connect Failed", "ota");
        else if (error == OTA_RECEIVE_ERROR)
            events.send("Recieve Failed", "ota");
        else if (error == OTA_END_ERROR)
            events.send("End Failed", "ota");
    });
    ArduinoOTA.setHostname(host);
    ArduinoOTA.begin();

    server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index)
            Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char *)data);
        if (final)
            Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    });
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (!index)
            Serial.printf("BodyStart: %u\n", total);
        Serial.printf("%s", (const char *)data);
        if (index + len == total)
            Serial.printf("BodyEnd: %u\n", total);
    });

    //}
    // WWW
    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    server.addHandler(&events);

    server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html").setFilter(ON_STA_FILTER);
    server.serveStatic("/", SPIFFS, "/ap/").setDefaultFile("index.html").setFilter(ON_AP_FILTER);

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Not found");
    });

    server.begin();
}
/*
void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}
*/
void loop()
{
    ArduinoOTA.handle();
    dns.processNextRequest();
    // put your main code here, to run repeatedly:
    strobe.handle();
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print WiFi IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
}