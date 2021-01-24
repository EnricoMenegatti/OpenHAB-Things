
void MQTT_Setup()
{
  String sub_topic_1 = "home/esp8266/";
  String sub_topic_2 = WiFi.localIP().toString();
  String sub_topic_3 = "/command";
  String sub_topic_4 = sub_topic_1 + sub_topic_2 + sub_topic_3;
  sub_topic_4.toCharArray(sub_topic, (sub_topic_4.length() + 1));
  
  String pub_topic_1 = "home/esp8266/";
  String pub_topic_2 = WiFi.localIP().toString();
  String pub_topic_3 = "/state";
  String pub_topic_4 = pub_topic_1 + pub_topic_2 + pub_topic_3;
  pub_topic_4.toCharArray(pub_topic, (pub_topic_4.length() + 1));

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTT_Callback);
  
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) 
    {
      Serial.println("connected");  
    } 
    else 
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void reconnect() 
{
  Serial.print("Attempting MQTT connection...");
  // Create a random client ID
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str(), "enrico", "Menegatti13")) 
  {
    Serial.println("connected");
    client.subscribe(sub_topic);
  } 
  else 
  {
    Serial.print("failed, rc=");
    Serial.print(client.state());
  }
}

void MQTT_Callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}
