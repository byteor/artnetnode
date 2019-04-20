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
        JsonObject &root = jsonBuffer.parseObject(file);
        file.close();
        if (!root.success()) {
            Serial.println("JSON parse error");
        }
        else
        {
            configFromJson(root);
            Serial.println("config loaded");
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
        configToJson(jsonBuffer).printTo(file);
        file.close();
        Serial.println("config file saved");
    }
}

void Config::serialize(String &to)
{
    configToJson(jsonBuffer).printTo(to);
}

bool Config::deserialize(String from)
{
    JsonObject &root = jsonBuffer.parseObject(from);
    return root.success() && configFromJson(root);
}
bool Config::deserialize(JsonObject &root)
{
    return root.success() && configFromJson(root);
}

JsonObject &Config::configToJson(JsonBuffer &doc)
{
    jsonBuffer.clear();
    JsonObject &root = doc.createObject();

    // Network
    root["host"] = host;

    JsonArray &array = root.createNestedArray("wifi");
    for (int i = 0; i < min(wifiCount, MAX_NETWORKS); i++)
    {
        JsonObject &net = array.createNestedObject();
        net["ssid"] = wifi[i].ssid;
        net["pass"] = wifi[i].pass;
        net["dhcp"] = wifi[i].dhcp;
    }
    // DMX
    JsonArray &array2 = root.createNestedArray("dmx");
    for (int i = 0; i < min(dmxCount, MAX_DMX_CHANNELS); i++)
    {
        JsonObject &ch = array2.createNestedObject();
        ch["channel"] = dmx[i].channel;
        ch["type"] = (int)dmx[i].type;
        ch["pin"] = dmx[i].pin;
        ch["pulse"] = dmx[i].pulse;
        ch["threshold"] = dmx[i].threshold;
    }
    return root;
}

bool Config::configFromJson(JsonObject &object)
{
    Serial.println("Parsing config...");
    // Networks
    host = object.get<String>("host");
    Serial.println("Host:" + host);
    JsonArray &nets = object["wifi"];
    for (WiFiNet net : wifi)
    {
        net.ssid = "";
        net.pass = "";
        net.dhcp = false;
    }
    Serial.println("Networks found:" + String(nets.size(), DEC));
    wifiCount = 0;
    for (JsonObject &net : nets)
    {
        if (wifiCount < MAX_NETWORKS)
        {
            wifi[wifiCount].ssid = net.get<String>("ssid");
            wifi[wifiCount].pass = net.get<String>("pass");
            wifi[wifiCount].dhcp = net.get<bool>("dhcp");
            Serial.println("Networks[" + String(wifiCount, DEC) + "]: " + wifi[wifiCount].ssid + " " + wifi[wifiCount].pass);
            wifiCount++;
        }
    }

    // DMX
    JsonArray &channels = object["dmx"];
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
    for (JsonObject &channel : channels)
    {
        if (dmxCount < MAX_DMX_CHANNELS && channel["type"] > 0)
        {
            dmx[dmxCount].type = static_cast<DmxType>(channel.get<int>("type"));
            dmx[dmxCount].channel = channel.get<uint8_t>("channel");
            dmx[dmxCount].threshold = channel.get<uint8_t>("threshold");
            dmx[dmxCount].pin = channel.get<uint8_t>("pin");
            dmx[dmxCount].pulse = channel.get<uint16_t>("pulse");
            Serial.println("DMX[" + String(dmxCount, DEC) + "]: " + String(dmx[dmxCount].type, DEC) + " " + String(dmx[dmxCount].channel, DEC));
            dmxCount++;
        }
    }
    return host && (dmxCount + wifiCount) > 0;
}
