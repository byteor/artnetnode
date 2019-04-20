#include <FS.h>
#include <ArduinoJson.h>

#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif

typedef struct
{
    String ssid;
    String pass;
    bool dhcp;
} WiFiNet;
#define MAX_NETWORKS 8

enum DmxType { Disabled = 0, Binary = 1, Dimmable = 2 };

typedef struct
{
    // DMX channel
    uint8_t channel;
    // DMX channel features
    DmxType type;
    // ON/OFF threshold for a DmxType::Binary switch (relay)
    uint8_t threshold;
    // Strobe pulse length
    uint16_t pulse;
    // Pin
    uint8_t pin;
} DmxChannel;
#define MAX_DMX_CHANNELS 4

class Config
{
    const int JSON_SIZE = 1024;
    String fileName;
    StaticJsonDocument<1024> jsonBuffer;
  protected:
    void configToJson(JsonDocument &doc);
    void configFromJson();
    int wifiCount;
    int dmxCount;
  public:
    // DMX
    DmxChannel dmx[MAX_DMX_CHANNELS];
    // WiFi
    WiFiNet wifi[MAX_NETWORKS];
    String host;

    Config();

    void load(String name);
    void save();
    void serialize(String &to);
    bool deserialize(String from);
};