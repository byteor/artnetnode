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
        DynamicJsonDocument doc(JSON_SIZE);
        configToJson(doc);
        serializeJsonPretty(doc, file);
        file.close();
        Serial.println("config file saved");
    }
}

void Config::serialize(String &to)
{
    configToJson(jsonBuffer);
    serializeJsonPretty(jsonBuffer, to);
}

bool Config::deserialize(String from)
{
    DeserializationError error = deserializeJson(jsonBuffer, from);
    if (error == DeserializationError::Ok)
    {
        configFromJson();
        Serial.println("config loaded");
        return true;
    }
    Serial.println("cannot parse config string");
    return false;
}

void Config::configToJson(JsonDocument &doc)
{
    doc.clear();
    // Network
    doc["host"] = host;

    JsonArray array = doc.createNestedArray("wifi");
    for (int i = 0; i < min(wifiCount, MAX_NETWORKS); i++)
    {
        JsonObject net = array.createNestedObject();
        net["ssid"] = wifi[i].ssid;
        net["pass"] = wifi[i].pass;
        net["dhcp"] = wifi[i].dhcp;
    }
    // DMX
    array = doc.createNestedArray("dmx");
    for (int i = 0; i < min(dmxCount, MAX_DMX_CHANNELS); i++)
    {
        JsonObject ch = array.createNestedObject();
        ch["channel"] = dmx[i].channel;
        ch["type"] = (int)dmx[i].type;
        ch["pin"] = dmx[i].pin;
        ch["pulse"] = dmx[i].pulse;
        ch["threshold"] = dmx[i].threshold;
    }
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
    Serial.println("Networks found:" + String(nets.size(), DEC));
    wifiCount = 0;
    for (JsonObject net : nets)
    {
        if (wifiCount < MAX_NETWORKS)
        {
            wifi[wifiCount].ssid = String(net["ssid"].as<char *>());
            wifi[wifiCount].pass = String(net["pass"].as<char *>());
            wifi[wifiCount].dhcp = net["dhcp"].as<bool>();
            Serial.println("Networks[" + String(wifiCount, DEC) + "]: " + wifi[wifiCount].ssid + " " + wifi[wifiCount].pass);
            wifiCount++;
        }
    }

    // DMX
    JsonArray channels = object["dmx"].as<JsonArray>();
    for (DmxChannel channel : dmx)
    {
        channel.channel = 0;
        channel.pin = 0;
        channel.pulse = 0;
        channel.type = DmxType::Disabled;
        channel.threshold = 0;
    }
    dmxCount = 0;
    Serial.println("DMX found:" + String(channels.size(), DEC));
    for (JsonObject channel : channels)
    {
        if (dmxCount < MAX_DMX_CHANNELS && channel["type"] > 0)
        {
            dmx[dmxCount].type = static_cast<DmxType>(channel["type"].as<int>());
            dmx[dmxCount].channel = channel["channel"].as<uint8_t>();
            dmx[dmxCount].threshold = channel["threshold"].as<uint8_t>();
            dmx[dmxCount].pin = channel["pin"].as<uint8_t>();
            dmx[dmxCount].pulse = channel["pulse"].as<uint16_t>();
            Serial.println("DMX[" + String(dmxCount, DEC) + "]: " + String(dmx[dmxCount].type, DEC) + " " + String(dmx[dmxCount].channel, DEC));
            dmxCount++;
        }
    }
}
