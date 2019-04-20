#include "board.h"
#include "AsyncJson.h"

// TODO: 1. add LED mode indicator
// TODO: 2. reset button to enforce Captive Portal (AP mode)

Board::Board()
{
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

void Board::start()
{
    Serial.begin(115200);
    Serial.println("");
    //Serial.setDebugOutput(true);
    SPIFFS.begin();

    // Parse config
    conf.load("config.json");

    // Start a captive portal if can't connect to WiFi
    bool connected = false;
    for(uint8_t counter = 0; counter < MAX_NETWORKS; counter++)
    {
        if(conf.wifi[counter].ssid.length() > 1 && conectWiFi(conf.wifi[counter].ssid, conf.wifi[counter].pass, conf.host))
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

    // Start mDNS // *** SUPER _ MEGA _ EVIL ***
    //MDNS.addService("http", "tcp", 80);
    //MDNS.begin(conf.host);
    //Serial.println("mdns is set up: " + conf.host);
    // OTA
    //Send OTA events to the browser
    /*
    ArduinoOTA.onStart([this]() { events->send("Update Start", "ota"); });
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


    server->onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index)
            Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char *)data);
        if (final)
            Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    });
    server->onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (!index)
            Serial.printf("BodyStart: %u\n", total);
        Serial.printf("%s", (const char *)data);
        if (index + len == total)
            Serial.printf("BodyEnd: %u\n", total);
    });
*/
    // Start web server
    server->on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    //server->addHandler(events);

    server->onNotFound([](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Not found");
    });

    setupConfigPages();

    server->begin();
    Serial.println("WWW is started up");
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

    // TODO: !
    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/config", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject& jsonObj = json.as<JsonObject>();
        Serial.printf("%s %s\r\n", request->methodToString(), request->url().c_str());
        String text;
        jsonObj.prettyPrintTo(text);
        Serial.println(text);
        int status = 200;
        if(conf.deserialize(jsonObj))
        {
            Serial.println("Updating config!");
            conf.save();
        }
        else
        {
            status = 400;
            Serial.println("Invalid config!");
        }

        AsyncWebServerResponse *response = request->beginResponse(status, "application/json", text);
        request->send(response);
    });
    handler->setMethod(HTTP_POST);
    server->addHandler(handler);

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

void Board::setSaveConfigCallback(SaveConfigCallback callback)
{
    saveConfigCallback = callback;
}


void Board::handle()
{
    //ArduinoOTA.handle();
    //dns.processNextRequest();
}

