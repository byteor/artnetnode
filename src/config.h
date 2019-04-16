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

class Config
{
    String fileName;
    StaticJsonDocument<512> jsonBuffer;
  protected:
    void configToJson();
    void configFromJson();
  public:
    uint8_t dmxChannel;
    WiFiNet wifi[MAX_NETWORKS];
    uint8_t wifiCount;
    String host;

    Config();

    void load(String name);
    void save();
    void serialize(String to);
    void deserialize(String from);
};