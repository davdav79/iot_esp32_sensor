#include "TinyMqtt.h"   // https://github.com/hsaturn/TinyMqtt
#include <map>

#define PORT 1883
MqttBroker broker(PORT);

/** Basic Mqtt Broker
  *
  *  +-----------------------------+
  *  | ESP                         |
  *  |       +--------+            |
  *  |       | broker |            | 1883 <--- External client/s
  *  |       +--------+            |
  *  |                             |
  *  +-----------------------------+
  *
  *  Your ESP will become a MqttBroker.
	*  You can test it with any client such as mqtt-spy for example
	*
  */

const char* ssid =  "Freebox-786A2F";
const char* password =  "usurpavi8dalum64lumine?";

void setup()
{
  Serial.begin(115200);

	if (strlen(ssid)==0)
		Console << TinyConsole::red << "****** PLEASE MODIFY ssid/password *************" << endl;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial << '.';
    delay(500);
  }
  Console << TinyConsole::green << "Connected to " << ssid << "IP address: " << WiFi.localIP() << endl;

  broker.begin();
  Console << "Broker ready : " << WiFi.localIP() << " on port " << PORT << endl;
}

template<class Type>
void display(std::string what, const Type& value, const char* unit="")
{
  static std::map<std::string, Type> last;
  if (last.find(what) == last.end() or last[what] != value)
  {
    Serial << what << ':' << value << unit << endl;
    last[what] = value;
  }
}

void loop()
{
  broker.loop();

  display("Broker clients", broker.clientsCount());
  display("Connected", broker.connected());
  for(auto client: broker.getClients())
  {
    std::string name=std::string("  ")+client->id();
    display(name+" id", name);
    display(name+" connected", client->connected());
    display(name+" local", client->isLocal());
    display(name+" keep alive", client->keepAlive());
  };
}
