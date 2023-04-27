#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define SDA 21 
#define SCL 22
#define SEALEVELPRESSURE_HPA (1013.25)

// TODO
const char* ssid = "ESP32 MQTT Broker";
const char* password = "";
WiFiClient wifiClient;

// MQTT Broker Daten
const char* mqttServer = "192.168.4.1";
PubSubClient mqttClient(wifiClient);

long zeitAlt = -9999;

// BME
Adafruit_BME680 bme;

// Callback Nachrichten Empfangen (subscribe)
void callback(char* topic, byte* message, unsigned int length)
{
  String str;
  for(int i = 0; i < length; i++)
  {
    str += (char)message[i];
  }
  Serial.print("Nachrichtenempfang für top: ");
  Serial.print(topic);
  Serial.print(". Nachricht: ");
  Serial.println(str);
}

// Verbindung zu mqtt Server herstellen
void mqttConnect()
{
  Serial.print("Verbindungsaufbau zum MQTT-Server ");
  Serial.print(mqttServer);

  while(!mqttClient.connected())
  {
    Serial.print(".");
    if(mqttClient.connect("ESP32Client"))
    {
      Serial.println();
      Serial.println("MQTT verbunden");
    }
    else
    {
      Serial.print("Fehlgeschlagen, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" erneuter Versuch in 5 Sekunden");
      delay(5000);
    }
  }
  mqttClient.subscribe("test/esp32");
}



void setup() {
  pinMode(15, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(921600);
  Serial.println("BME680 Test");
  // Wire.begin();
  // i2cScan.begin(SDA, SCL, 460800);
  // Serial.println("Hello from the setup");

  // Tries to find bme680 sensor
  if(!bme.begin())
  {
    Serial.println("Couldn't find valid BME680 sensor");
    while(1);
  }

  // Set oversampling and filter initialization
  bme.setGasHeater(320, 150);
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);

  // MQTT Stuff
  // Verbindung WiFi
  Serial.println();
  Serial.print("Verbindungdaufbau zu ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Verbindung MQTT Server
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(callback);
  mqttConnect();
}

void loop() {
  // Start zur Augabe über Uart und Blinke LED
  // put your main code here, to run repeatedly:
  // digitalWrite(15, HIGH);
  // delay(1000);
  // Serial.println("Hello from loop");
  // digitalWrite(15, LOW);
  // delay(1000);

  // Scannen von I2C Geräten
  // Serial.println("Start I2C-Scan Kanal 1");
  // scan(1);
  // delay(100);
  // Serial.println("Start I2C Scan Kanal 2");
  // scan(2);
  // delay(5000);

  // BME 680 Data
  
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();
  

  // MQTT STUFF
  if(!mqttClient.connected())
  {
    mqttConnect();
  }
  mqttClient.loop();

  if(millis() - zeitAlt > 3000)
  {
    char txString[128];
    //zeitAlt = millis();
    //sprintf(txString, "Zeitstempel %d", millis()/1000);

    // Versand Temperatur
    sprintf(txString, "%.2lf", bme.temperature);
    Serial.println(String("Versand Temperatur: ") + txString);
    mqttClient.publish("esp32/temp", txString);

    // Versand Pressure
    sprintf(txString, "%.2lf", (bme.pressure / 100.0));
    Serial.println(String("Versand Luftdruck: ") + txString);
    mqttClient.publish("esp32/pres", txString);

    // Versand Humidity
    sprintf(txString, "%.2lf", bme.humidity);
    Serial.println(String("Versand Luftfeuchtigkeit: ") + txString);
    mqttClient.publish("esp32/hum", txString);


  }
  delay(2000);
}