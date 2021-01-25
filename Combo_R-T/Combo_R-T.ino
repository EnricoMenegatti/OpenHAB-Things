
#define REFRESH_MIN 10
#define WIFI_CONN_TIME 30
#define MQTT_CONN_TIME 30

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "DHTesp.h"

//ESP----------------------------------------------------------------------------------------------------------------
bool resetESP = false, allSetup = false, serverAP = false;
bool saveInputPinState, saveOutputPinState;
int outputPin, inputPin;
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
  
  String payload = "{\"temperature\":" +String(temperature)+ ",\"humidity\":" +String(humidity)+ ", \"input\":" +String(saveInputPinState)+ ",\"output\":" +String(saveOutputPinState)+ "}";
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
  
//try to connect Wi-Fi
  if (WiFiSTA_Setup())
  {
    wifiConnected = true;
    OTA_Setup("esp8266");
    if (MQTT_Setup()) mqttConnected = true;
  }
  
// I/O
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  dht.setup(DHTPIN, DHTesp::DHT22);
  
  saveInputPinState = digitalRead(inputPin);
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

  if(saveInputPinState != digitalRead(inputPin)) //on toggle
  {
    delay(150);
    if(saveInputPinState != digitalRead(inputPin)) //antirimbalzo
    {
      if(saveOutputPinState == true) 
      {
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(outputPin, LOW);
        saveOutputPinState = false;
      }
      else
      {        
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(outputPin, HIGH);
        saveOutputPinState = true;
      }
      
      saveInputPinState = digitalRead(inputPin);
      Serial.print("INPUT toggle to: "); Serial.println(saveInputPinState);
      Serial.print("OUTPUT toggle to: "); Serial.println(saveOutputPinState);
      Publish();
    }
  }

//se la connessione avviene a setup terminato riavvia
  if (!wifiConnected)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      delay(1);
      ESP.restart(); //ESP.reset();
    }
  }

  ArduinoOTA.handle();
  client.loop();
  delay(1);
}
