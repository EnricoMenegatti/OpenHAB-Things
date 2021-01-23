
#include <ESP8266WiFi.h>
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

//FUNCTIONS----------------------------------------------------------------------------------------------------------------

//SETUP--------------------------------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("Setup...");

  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Password: "); Serial.println(password);
  
//try to connect Wi-Fi
  if(WiFiSTA_Setup()) allSetup = true;
  OTA_Setup("esp8266");
  
// I/O
  inputPin = 16;//D0 default
  outputPin = 5;//D1 default

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);

  saveInputPinState = digitalRead(inputPin);
}

//MAIN---------------------------------------------------------------------------------------------------------------------
void loop()
{ 
  if(saveInputPinState != digitalRead(inputPin)) //on toggle
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
  }

  ArduinoOTA.handle();
  delay(1);
}
