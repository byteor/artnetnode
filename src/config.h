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
    // Strobe period multiplier
    uint16_t multiplier;
    // Pin
    uint8_t pin;
} DmxChannel;
#define MAX_DMX_CHANNELS 4

typedef struct
{
  // PWM frequency
  uint16_t  pwmFreq = 200;  // ESP8266 default is 1000 which may cause MOSFER overheat
} HardwareConfig;

class Config
{
    const int JSON_SIZE = 1024;
    String fileName;
    //StaticJsonBuffer<2048> jsonBuffer;
    DynamicJsonBuffer jsonBuffer;
  protected:
    JsonObject& configToJson(JsonBuffer &doc);
    bool configFromJson(JsonObject& object);
    int wifiCount;
    int dmxCount;
  public:
    // DMX
    DmxChannel dmx[MAX_DMX_CHANNELS];
    // WiFi
    WiFiNet wifi[MAX_NETWORKS];
    String host;
    HardwareConfig hardware;

    Config();

    void load(String name);
    void save();
    void serialize(String &to);
    bool deserialize(String from);
    bool deserialize(JsonObject& object);
};