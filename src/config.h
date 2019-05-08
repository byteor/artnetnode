#include <FS.h>
#include <ArduinoJson.h>

#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif

#define DEFAULT_PWM_FREQ 600
#define DEFAULT_HOST_NAME "ArtNetNode"

#define DEFAULT_DMX_CHANNEL 1
#define DEFAULT_DMX_PULSE 20
#define DEFAULT_DMX_MULIPLIER 5

typedef struct
{
  String ssid;
  String pass;
  bool dhcp;
  uint8_t order;
} WiFiNet;
#define MAX_NETWORKS 8

enum DmxType
{
  Disabled = 0,
  Binary = 1,
  Dimmable = 2
};

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
  // Active level: 0=LOW, 1=HIGH
  uint8_t level;
} DmxChannel;
#define MAX_DMX_CHANNELS 4

typedef struct
{
  // PWM frequency
  uint16_t pwmFreq = DEFAULT_PWM_FREQ; // ESP8266 default is 1000 which may cause MOSFER overheat
} HardwareConfig;

class Config
{
  const int JSON_SIZE = 1024;
  String fileName;
  DynamicJsonBuffer jsonBuffer;

protected:
  const String DMX_DISABLED = String("DISABLED");
  const String DMX_BINARY = String("BINARY");
  const String DMX_DIMMABLE = String("DIMMABLE");

  JsonObject &configToJson(JsonBuffer &doc);
  bool configFromJson(JsonObject &object);
  void loadDefault();
  void cleanup();
  int wifiCount;
  int dmxCount;

  DmxType dmxTypeFromString(String type)
  {
    if (DMX_BINARY.equals(type))
      return DmxType::Binary;
    if (DMX_DIMMABLE.equals(type))
      return DmxType::Dimmable;
    return DmxType::Disabled;
  }

  String dmxTypeToString(DmxType type)
  {
    if (type == DmxType::Binary)
      return DMX_BINARY;
    if (type == DmxType::Dimmable)
      return DMX_DIMMABLE;
    return DMX_DISABLED;
  }

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
  bool deserialize(JsonObject &object);
};