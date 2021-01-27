
boolean WiFiSTA_Setup() 
{
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

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

void WiFiAP_Setup() 
{
  WiFi.softAPConfig(IP_AP, GTW_AP, mask_AP);
  WiFi.softAP(ssid_AP, password_AP);

  Serial.println();
  Serial.print("AP SSID: ");
  Serial.println(ssid_AP);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}