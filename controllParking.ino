#include "WIFIconfig.h"
#include "MQTTconfig.h"
#include "DHT.h"
#include "RadarSensor.h"
#include "Light.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Timer.h>

#define DHTPIN 27
#define DHTTYPE DHT11
#define RADAR_SENSOR_PIN 32
#define LIGHT_PIN 16

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
RadarSensor radarSensor(RADAR_SENSOR_PIN);
Light light(LIGHT_PIN);

// CONSTANTS:
const boolean START = true;
const boolean RESET = false;
const char *MANUAL = "MANUAL";
const char *AUTOMATIC = "AUTOMATIC";

// TIMERS:
TON *tPublishInfo;
TON *tCheckConnection;
TON *tCheckDHT;
TON *tAutomaticLightOn;

void setup()
{
//  Serial.begin(115200);

  // Encender rele:
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(5000);

  WIFIConnection();

  OTAConfig();

  MQTTConnection();

  Serial.println("Connected to the WiFi network");

  dht.begin();
  radarSensor.begin();
  light.begin();

  tPublishInfo = new TON(1000);
  tCheckConnection = new TON(52000);
  tCheckDHT = new TON(52000);
  tAutomaticLightOn = new TON(120000);

  digitalWrite(16, HIGH);
}

void publishInfo(float dhtData[], boolean estadoLuz)
{
  if (tPublishInfo->IN(START))
  {
    StaticJsonDocument<192> jsonDoc;
    JsonObject recorrido = jsonDoc.createNestedObject("recorrido");

    String payload = "";
    char *estado = "x";

    jsonDoc["temperatura"] = dhtData[0];
    jsonDoc["humedad"] = dhtData[1];
    jsonDoc["humedadRelativa"] = dhtData[2];

    jsonDoc["luz"]["estado"] = estadoLuz;

    serializeJson(jsonDoc, payload);
    client.publish(topicState, (char *)payload.c_str());

    tPublishInfo->IN(RESET);
  }
}

void readDataFromDHT()
{
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}

void checkMqttConnection()
{
  if (tCheckConnection->IN(START))
  {
    if (!client.connected())
    {
      ESP.restart();
    }

    tCheckConnection->IN(RESET);
  }
}

void loop()
{
  ArduinoOTA.handle();
  client.loop();
  yield();

  float dhtData[3];

  dhtData[0] = dht.readTemperature();
  dhtData[1] = dht.readHumidity();
  dhtData[2] = dht.computeHeatIndex(dhtData[0], dhtData[1], false);

  publishInfo(dhtData, light.getState());

  if (radarSensor.read() && tPublishInfo->IN(START) && light.getMode() == AUTOMATIC)
  {
    light.turnOn();
    tPublishInfo->IN(RESET);
  }
}

void WIFIConnection()
{
  // connecting to a WiFi network
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi..");
    delay(15000);
    ESP.restart();
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void OTAConfig()
{
  ArduinoOTA.setHostname(client_name);
  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    } });
  ArduinoOTA.begin();
}

void MQTTConnection()
{
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected())
  {
    String client_id = client_name;
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe(topicCommand);
}

void lightControl(bool night, String mode, bool manualOn)
{
  light.setMode(mode);

  if (mode == MANUAL && !manualOn)
  {
    light.turnOff();

    return;
  }

  if (mode == MANUAL && manualOn)
  {
    light.turnOn();

    return;
  }
}

void callback(char *topicCommand, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topicCommand);
  Serial.print("Message:");
  String payload_n;

  for (int i = 0; i < length; i++)
  {
    payload_n += (char)payload[i];
  }

  Serial.println(payload_n);
  Serial.println("-----------------------");

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload_n);
  if (error)
    return;
  String mode = doc["luz"]["modo"];
  bool night = doc["luz"]["noche"];
  bool manualOn = doc["luz"]["on"];
  lightControl(night, mode, manualOn);
}
