
boolean WiFiSTA_Setup() 
{
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    if (i > WIFI_CONN_TIME * 2)
    {
      state = false; 
      break;
    }
    i++;
  }
  Serial.println("");
  if (state)
  {
    Serial.print("Connected to "); Serial.println(ssid);
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
  }
  else 
  {
    Serial.println("Connection failed.");
  }
  return state;
}
