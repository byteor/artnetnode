#include "board.h"
#include "AsyncJson.h"

// TODO: 1. add LED mode indicator
// TODO: 2. reset button to enforce Captive Portal (AP mode)

Board::Board()
{
    udpHandler = new UdpHandler(&Serial);
}

bool Board::isIp(String str)
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
String Board::toStringIp(IPAddress ip)
{
    String res = "";
    for (int i = 0; i < 3; i++)
    {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}

void Board::printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print WiFi IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
}

void Board::restart()
{
    isRestartPending = true;
}

void Board::start()
{
    Serial.begin(115200);
    Serial.println("starting up...");
    //Serial.setDebugOutput(true);
    SPIFFS.begin();
    // Parse config
    conf.load(CONFIG_FILE);

    delay(200);

    // Start a captive portal if can't connect to WiFi
    bool connected = false;
    for (uint8_t counter = 0; counter < MAX_NETWORKS; counter++)
    {
        if (conf.wifi[counter].ssid.length() > 1 && conectWiFi(conf.wifi[counter].ssid, conf.wifi[counter].pass, conf.host))
        {
            connected = true;
            break;
        }
    }
    if (!connected)
        startCaptivePortal();

    Serial.println("connected :)");
    Serial.println();
    printWifiStatus();

    // Start mDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.begin(conf.host);
    Serial.println("mDNS is set up: " + conf.host);
    // OTA
    //Send OTA events to the browser

    /*  ArduinoOTA.onStart([this]() { events->send("Update Start", "ota"); });
    ArduinoOTA.onEnd([this]() { events->send("Update End", "ota"); });
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        char p[32];
        sprintf(p, "Progress: %u%%\n", (progress / (total / 100)));
        events->send(p, "ota");
    });
    ArduinoOTA.onError([this](ota_error_t error) {
        if (error == OTA_AUTH_ERROR)
            events->send("Auth Failed", "ota");
        else if (error == OTA_BEGIN_ERROR)
            events->send("Begin Failed", "ota");
        else if (error == OTA_CONNECT_ERROR)
            events->send("Connect Failed", "ota");
        else if (error == OTA_RECEIVE_ERROR)
            events->send("Recieve Failed", "ota");
        else if (error == OTA_END_ERROR)
            events->send("End Failed", "ota");
    });
    ArduinoOTA.setHostname(conf.host.c_str());
    ArduinoOTA.begin();
    Serial.println("OTA is set up");
   server->addHandler(events); */

    // Start web server
    server->on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    setupConfigPages();

    server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    server->begin();
    Serial.println("WWW is started up");

    // PWM parameters
    analogWriteRange(255);
    if (conf.hardware.pwmFreq)
    {
        analogWriteFreq(conf.hardware.pwmFreq);
    }
    // DMX
    for (int i = 0; i < MAX_DMX_CHANNELS; i++)
    {
        DmxChannel ch = conf.dmx[i];
        if (ch.type != DmxType::Disabled)
        {
            strobes[i] = new Strobe(ch.pin, ch.pulse, ch.level, ch.type == DmxType::Dimmable);
        }
        else
        {
            strobes[i] = NULL;
        }
    }

    if (!udpHandler->start(UDP_PORT, [this](int channel, int value) {
            for (int i = 0; i < MAX_DMX_CHANNELS; i++)
            {
                // data array is zero-based when DMX channels start with "1"
                // First channel controls the main light 'ON/OFF' or 'Dimmer'
                // The second channel controls stroboscope
                DmxChannel ch = conf.dmx[i];
                if (channel == ch.channel - 1 && ch.type != DmxType::Disabled)
                {
                    Serial.print("[");
                    Serial.print(ch.channel);
                    Serial.print("]=");
                    Serial.print(value);
                    Serial.print(" /value");
                    Serial.println();
                    if (ch.type == DmxType::Binary)
                    {
                        if (value >= ch.threshold)
                        {
                            Serial.println("ON");
                            strobes[i]->start();
                        }
                        else
                        {
                            Serial.println("OFF");
                            strobes[i]->stop();
                        }
                    }
                    else if (ch.type == DmxType::Dimmable)
                    {
                        value == 0 ? strobes[i]->stop() : strobes[i]->start(value);
                    }
                }
                if (channel == ch.channel && ch.pulse > 0)
                {
                    Serial.print("[");
                    Serial.print(ch.channel + 1);
                    Serial.print("]=");
                    Serial.print(value);
                    Serial.print(" /pulse");
                    Serial.println();
                    strobes[i]->setDuration(ch.pulse);
                    strobes[i]->setInterval(value * ch.multiplier);
                }
            }
        }))
    {
        Serial.print("couldn't start UDP");
    }
}

void Board::setupConfigPages()
{
    server->serveStatic("/", SPIFFS, "/www/");
    server->serveStatic("/fs/", SPIFFS, "/fs/");
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("YO:/");
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
        response->addHeader("Location", String("/index.html"));
        request->send(response);
    });

    server->on("/config", HTTP_GET, [this](AsyncWebServerRequest *request) {
        Serial.println("GET /config");
        String json;
        conf.serialize(json);
        Serial.println(json);
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
        request->send(response);
    });

    server->on("/restart", HTTP_POST, [this](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\": \"OK\"}");
        request->send(response);
        restart();
    });

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/config", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject &jsonObj = json.as<JsonObject>();
        Serial.printf("%s %s\r\n", request->methodToString(), request->url().c_str());
        String text;
        int status = 200;
        Config tempConf;
        tempConf.load(CONFIG_FILE);
        if (tempConf.deserialize(jsonObj))
        {
            tempConf.save();
            tempConf.serialize(text);
            Serial.println(text);
        }
        else
        {
            status = 400;
            jsonObj.prettyPrintTo(text);
            Serial.println("Invalid config!");
        }

        AsyncWebServerResponse *response = request->beginResponse(status, "application/json", text);
        request->send(response);
    });
    handler->setMethod(HTTP_POST);
    server->addHandler(handler);

    AsyncCallbackJsonWebHandler *wifiHandler = new AsyncCallbackJsonWebHandler("/wifi", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject &jsonObj = json.as<JsonObject>();
        Serial.printf("%s %s\r\n", request->methodToString(), request->url().c_str());
        String text;
        int status = 200;
        Config tempConf;
        tempConf.load(CONFIG_FILE);
        if (tempConf.deserialize(jsonObj))
        {
            tempConf.save();
            tempConf.serialize(text);
            Serial.println(text);
        }
        else
        {
            status = 400;
            jsonObj.prettyPrintTo(text);
            Serial.println("Invalid config!");
        }

        AsyncWebServerResponse *response = request->beginResponse(status, "application/json", text);
        request->send(response);
    });
    wifiHandler->setMethod(HTTP_POST);
    server->addHandler(wifiHandler);
}

// Captive Portal
void Board::startCaptivePortal()
{
    dns.stop();
    server->reset();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(conf.host.c_str(), AP_PASS);
    delay(500);
    Serial.println(F("AP IP address: "));
    Serial.println(WiFi.softAPIP());
    // Setup the DNS server redirecting all the domains to the apIP
    dns.setErrorReplyCode(DNSReplyCode::NoError);
    dns.start(DNS_PORT, "*", WiFi.softAPIP());

    server->onNotFound([this](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
        response->addHeader("Location", String("http://") + toStringIp(request->client()->localIP()));
        request->send(response);
    });
    setupConfigPages();
    server->begin();
    while (true)
    {
        dns.processNextRequest();
        if (isRestartPending)
        {
            Serial.println("Reboot...");
            delay(200);
            ESP.restart();
        }
    }
}

bool Board::conectWiFi(String ssid, String pass, String host)
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

void Board::handle()
{
    //ArduinoOTA.handle();
    if (isRestartPending)
    {
        Serial.println("Reboot...");
        for (Strobe *strobe : strobes)
            if (strobe)
                strobe->stop();
        delay(200);
        ESP.restart();
    }
    for (Strobe *strobe : strobes)
        if (strobe)
            strobe->handle();
}
