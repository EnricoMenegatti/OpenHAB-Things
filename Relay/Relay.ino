
#define REFRESH_MIN 10
#define WIFI_CONN_TIME 30
#define MQTT_CONN_TIME 30

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ArduinoOTA.h>

//ESP----------------------------------------------------------------------------------------------------------------
bool resetESP = false, allSetup = false, serverAP = false;
bool saveInputPinState, saveOutputPinState;
int outputPin, inputPin;
double thisTime, lastTime;

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
  String payload = "{\"input\":" +String(saveInputPinState)+ ",\"output\":" +String(saveOutputPinState)+ "}";
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
  
// I/O
  inputPin = readFile(SPIFFS, "/configInput.txt").toInt();
  outputPin = readFile(SPIFFS, "/configOutput.txt").toInt();
  inputPin = 16;//D0 default
  outputPin = 5;//D1 default

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  
  saveInputPinState = digitalRead(inputPin);
  lastTime = millis();
}

//MAIN---------------------------------------------------------------------------------------------------------------------
void loop()
{
  thisTime = millis();
  if (thisTime - lastTime > (REFRESH_MIN * 60 * 1000))
  {
    if (mqttConnected)
    {
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
