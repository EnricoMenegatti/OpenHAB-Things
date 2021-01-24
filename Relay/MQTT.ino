
boolean MQTT_Setup()
{
  boolean state = true;
  int i = 0;
  
  String sub_topic_1 = "home/esp8266/";
  String sub_topic_2 = WiFi.localIP().toString();
  String sub_topic_3 = "/command";
  String sub_topic_4 = sub_topic_1 + sub_topic_2 + sub_topic_3;
  sub_topic_4.toCharArray(sub_topic, (sub_topic_4.length() + 1));
  
  String pub_topic_1 = "home/esp8266/";
  String pub_topic_2 = WiFi.localIP().toString();
  String pub_topic_3 = "/status";
  String pub_topic_4 = pub_topic_1 + pub_topic_2 + pub_topic_3;
  pub_topic_4.toCharArray(pub_topic, (pub_topic_4.length() + 1));

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTT_Callback);

  String clientId = "ESP8266Client-";
  clientId += WiFi.localIP().toString();
  Serial.println("Connecting to MQTT...");
    
  while (!client.connected()) 
  {  
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) 
    {
      Serial.println("connected");
      client.subscribe(sub_topic);
    } 
    else 
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      
      if (i > MQTT_CONN_TIME * 2)
      {
        state = false;
        break;
      }
      i++;
      delay(500);
    }
  }
  return state;
}

void MQTT_Callback(char* topic, byte* payload, unsigned int length) 
{
  char payload_char[length];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    payload_char[i] = (char)payload[i]; 
    Serial.print(payload_char[i]);
  }
  Serial.println();

  if (String(payload_char) == "on")
  {
    Serial.println("ON");
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(outputPin, HIGH);
    saveOutputPinState = true;
  }

  else if (String(payload_char) == "off")
  {
    Serial.println("OFF");
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(outputPin, LOW);
    saveOutputPinState = false;
  }

  else if (String(payload_char) == "refresh")
  {
    Publish();
  }
}
