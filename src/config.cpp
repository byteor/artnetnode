#include "config.h"

Config::Config()
{
}

void Config::load(String name)
{
    fileName = String("/config/");
    fileName += name;
    File file = SPIFFS.open(fileName, "r");
    if (!file)
    {
        Serial.println("no config file");
    }
    else
    {
        DeserializationError error = deserializeJson(jsonBuffer, file);
        file.close();
        if (error == DeserializationError::Ok)
        {
            configFromJson();
            Serial.println("config loaded");
        }
        else
        {
            Serial.println("cannot parse config file");
        }
    }
}

void Config::configToJson()
{
    JsonObject object = jsonBuffer.as<JsonObject>();
    // Network
    object["host"] = host;
    JsonArray array = object.createNestedArray("wifi");
    for (int i = 0; i < wifiCount; i++)
    {
        JsonObject net = array.createNestedObject();
        net["ssid"] = wifi[i].ssid;
        net["pass"] = wifi[i].pass;
        net["dhcp"] = wifi[i].dhcp;
    }
    // DMX
    jsonBuffer["dmxChannel"] = dmxChannel;
}

void Config::configFromJson()
{
    JsonObject object = jsonBuffer.as<JsonObject>();
    Serial.println("Parsing config...");
    // Networks
    host = String(object["host"].as<char *>());
    Serial.println("Host:" + host);
    JsonArray nets = object["wifi"].as<JsonArray>();
    for (WiFiNet net : wifi)
    {
        net.ssid = "";
        net.pass = "";
        net.dhcp = false;
    }
    uint8_t netCount = 0;
    Serial.println("Networks found:" + String(nets.size(), DEC));
    for (JsonObject net : nets)
    {
        if (netCount < MAX_NETWORKS)
        {
            wifi[netCount].ssid = String(net["ssid"].as<char *>());
            wifi[netCount].pass = String(net["pass"].as<char *>());
            wifi[netCount].dhcp = net["dhcp"].as<bool>();
            Serial.println("Networks[" + String(netCount, DEC) + "]: " + wifi[netCount].ssid + " " + wifi[netCount].pass);
            netCount++;
        }
    }
    // DMX
    dmxChannel = object["dmxChannel"];
    Serial.println("DMX channel:" + String(dmxChannel, DEC));
}

void Config::save()
{
    Serial.println("saving board config...");
    File file = SPIFFS.open(fileName, "w");
    if (!file)
    {
        Serial.println("cannot save config file");
    }
    else
    {
        configToJson();
        serializeJsonPretty(jsonBuffer, file);
        file.close();
        Serial.println("config file saved");
    }
}

void Config::serialize(String to)
{
    configToJson();
    serializeJsonPretty(jsonBuffer, to);
}

void Config::deserialize(String from)
{
    DeserializationError error = deserializeJson(jsonBuffer, from);
    if (error == DeserializationError::Ok)
    {
        configFromJson();
        Serial.println("config loaded");
    }
    else
    {
        Serial.println("cannot parse config string");
    }
}