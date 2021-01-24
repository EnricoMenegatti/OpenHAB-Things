
#define REFRESH_MIN 0.1

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "DHTesp.h"

//ESP----------------------------------------------------------------------------------------------------------------
bool resetESP = false, allSetup = false, serverAP = false;
double thisTime, lastTime;
float temperature, humidity;

//DHT----------------------------------------------------------------------------------------------------------------
#define DHTPIN D4     // what digital pin the DHT22 is conected to

DHTesp dht;

//WI-FI----------------------------------------------------------------------------------------------------------------
bool wifiConnected = false;
char ssid[40] = "Vodafone-Menegatti";//Vodafone-Menegatti
char password[40] = "Menegatti13";//Menegatti13

//MQTT----------------------------------------------------------------------------------------------------------------
const char* mqtt_server = "localhost";
const char* sub_topic = "home/esp8266/temperature_req";
const char* pub_topic = "home/esp8266/temperature";
const int port = 1883;

#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];

WiFiClient espClient;
PubSubClient client(espClient);

//FUNCTIONS----------------------------------------------------------------------------------------------------------------

//SETUP--------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("Setup...");

  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Password: "); Serial.println(password);
  
//try to connect Wi-Fi
  if(WiFiSTA_Setup())
  {
    allSetup = true;
  }
  OTA_Setup("esp8266");
  MQTT_Setup();
  
// I/O
  pinMode(LED_BUILTIN, OUTPUT);
  dht.setup(DHTPIN, DHTesp::DHT22);
  
  lastTime = millis();
}

//MAIN---------------------------------------------------------------------------------------------------------------------
void loop()
{
//verify mqtt broker connection
  //if (!client.connected()) reconnect();

  thisTime = millis();
  if(thisTime - lastTime > (REFRESH_MIN * 60 * 1000))
  {
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
    
    String payload = "{\"temperature\":" +String(temperature)+ ",\"humidity\":" +String(humidity)+ "}";
    payload.toCharArray(msg, (payload.length() + 1));
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(pub_topic, msg);
    
    lastTime = millis();
  }

  ArduinoOTA.handle();
  client.loop();
  delay(1);
}
