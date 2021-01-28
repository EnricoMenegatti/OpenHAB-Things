
#define REFRESH_MIN 10
#define WIFI_CONN_TIME 30
#define MQTT_CONN_TIME 30

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include "DHTesp.h"

//ESP----------------------------------------------------------------------------------------------------------------
bool resetESP = false, allSetup = false, serverAP = false;
int dhtPin;
double thisTime, lastTime;
float temperature, humidity;

//DHT----------------------------------------------------------------------------------------------------------------
char sensor_type[40];
DHTesp dht;

//WI-FI----------------------------------------------------------------------------------------------------------------
bool wifiConnected = false;
char ssid[40] = "Vodafone-Menegatti";//Vodafone-Menegatti
char password[40] = "Menegatti13";//Menegatti13

const char* ssid_AP = "ESP8266";
const char* password_AP = "esp8266";
IPAddress IP_AP(192,168,1,1);
IPAddress mask_AP = (255, 255, 255, 0);
IPAddress GTW_AP(192,168,1,1);

//WEBSERVER----------------------------------------------------------------------------------------------------------------
AsyncWebServer server(80);

//MQTT----------------------------------------------------------------------------------------------------------------
bool mqttConnected = false;
const char* mqtt_server = "192.168.1.100";
const int mqtt_port = 1883;
const char* mqtt_user = "enrico";
const char* mqtt_password = "Menegatti13";

char sub_topic[100];
char pub_topic[100];
char msg[100];

WiFiClient espClient;
PubSubClient client(espClient);

//FUNCTIONS----------------------------------------------------------------------------------------------------------------

void Publish()
{
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  
  String payload = "{\"temperature\":" +String(temperature)+ ",\"humidity\":" +String(humidity)+ "}";
  payload.toCharArray(msg, (payload.length() + 1));
  Serial.print("Publish message [");
  Serial.print(pub_topic);
  Serial.print("] ");
  Serial.println(msg);
  client.publish(pub_topic, msg);
}

//SETUP--------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("Setup...");
  
  SPIFFS_Setup();

  readFile(SPIFFS, "/configSSID.txt").toCharArray(ssid, 40);
  readFile(SPIFFS, "/configPassword.txt").toCharArray(password, 40);
  if(String(ssid).length() <= 1 || String(password).length() <= 1) 
  {
    Serial.println("Error reading WiFi files!");
  }

  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Password: "); Serial.println(password);
  
//try to connect Wi-Fi
  if (WiFiSTA_Setup())
  {
    Start_Server();

    wifiConnected = true;
    OTA_Setup("esp8266");
    if (MQTT_Setup()) mqttConnected = true;
  }
  else //start AP Wi-Fi
  {
    WiFiAP_Setup();
    Start_Server();
    server.begin(); // start the HTTP server

    serverAP = true;
    lastTime = millis();
  }
  
// DHT
  readFile(SPIFFS, "/configSensor.txt").toCharArray(sensor_type, 40);
  if (String(sensor_type) == "DHT11")
  {
    dhtPin = readFile(SPIFFS, "/configDHT_pin.txt").toInt();
    if (dhtPin == 0) dhtPin = 4;//D2 default

    dht.setup(dhtPin, DHTesp::DHT11);
  }
  else if (String(sensor_type) == "DHT22")
  {
    dhtPin = readFile(SPIFFS, "/configDHT_pin.txt").toInt();
    if (dhtPin == 0) dhtPin = 4;//D2 default

    dht.setup(dhtPin, DHTesp::DHT22);
  }

// I/O
  pinMode(LED_BUILTIN, OUTPUT);
  
  lastTime = millis();
  Publish();
}

//MAIN---------------------------------------------------------------------------------------------------------------------
void loop()
{
  thisTime = millis();
  if (thisTime - lastTime > (REFRESH_MIN * 60 * 1000))
  {
    if (mqttConnected)
    {
      Publish();
    }
    else
    {
      if (MQTT_Setup()) mqttConnected = true;
    }
      lastTime = millis();
  }

//se la connessione avviene a setup terminato riavvia
  if (!wifiConnected)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      delay(1);
      ESP.restart();
    }
  }

  if(resetESP)
  {
    delay(1);
    ESP.restart(); //ESP.reset();
  }

  ArduinoOTA.handle();
  client.loop();
  delay(1);
}
