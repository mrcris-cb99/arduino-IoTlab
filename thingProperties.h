#include "arduino_secrets.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]     = SECRET_SSID;
const char PASS[]     = SECRET_OPTIONAL_PASS;


extern String overallStatus;
extern float temperature;
extern int airQuality;
extern int normalizedValueLDR;
extern bool presence;

void initProperties(){

  ArduinoCloud.addProperty(overallStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(temperature, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(airQuality, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(normalizedValueLDR, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(presence, READ, ON_CHANGE, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
