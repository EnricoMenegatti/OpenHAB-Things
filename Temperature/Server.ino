
void Start_Server() // Start a HTTP server with a file read handler and an upload handler
{ 
  server.on("/style.css", [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/SubmitRefresh", [] (AsyncWebServerRequest *request) {
    Serial.println("Submit Refresh");
    
    Publish();
    
    request->redirect("/Info.html");
  });
  
  server.on("/SubmitReset", [] (AsyncWebServerRequest *request) {
    Serial.println("Resetting ESP");
    request->redirect("/SuccessReset.html");
    resetESP = true;
  });
  
  server.on("/SubmitRipr", [] (AsyncWebServerRequest *request) {
    Serial.println("Riprisino Submit");
    if (deleteFile(SPIFFS, "/configSSID.txt") && deleteFile(SPIFFS, "/configPassword.txt") && deleteFile(SPIFFS, "/configDevice.txt"))
      request->send(SPIFFS, "/SuccessRipr.html", "text/html");
  });

  server.on("/submit", HTTP_POST, [] (AsyncWebServerRequest *request) {
    Serial.println("Config Submit");
    int params = request->params();
    for (int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size()); 
      else if (p->isPost()) Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      else Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());

      if (p->name() == "ssid") 
      {
        request->arg("ssid").toCharArray(ssid, 40);
        writeFile(SPIFFS, "/configSSID.txt", ssid);
      }
      
      else if (p->name() == "pw") 
      {
        request->arg("pw").toCharArray(password, 40);
        writeFile(SPIFFS, "/configPassword.txt", password);
      }
      
      else if (p->name() == "sensor") 
      {
        request->arg("sensor").toCharArray(sensor_type, 40);
        writeFile(SPIFFS, "/configSensor.txt", sensor_type);
      }
      
      else if (p->name() == "dht_pin") 
      {
        dhtPin = request->arg("dht_pin").toInt();
        writeFile(SPIFFS, "/configDHT_pin.txt", String(dhtPin).c_str());
      }

      else if (p->name() == "dht_hum_offset") 
      {
        dhtHumOffset = request->arg("dht_hum_offset").toFloat();
        writeFile(SPIFFS, "/configDHT_HumOffset.txt", String(dhtHumOffset).c_str());
      }
      
      else if (p->name() == "dht_temp_offset") 
      {
        dhtTempOffset = request->arg("dht_temp_offset").toFloat();
        writeFile(SPIFFS, "/configDHT_TempOffset.txt", String(dhtTempOffset).c_str());
      }

      else 
      {
        request->redirect("/Error.html");
        return;
      }
    }
    request->redirect("/SuccessFile.html");
  });
  
  server.onNotFound([](AsyncWebServerRequest *request){
    String path = request->url().c_str();
    Serial.println("handleFileRead: " + path);

    if (path.endsWith("/")) path += "Info.html"; // If a folder is requested, send the index file
    String pathWithGz = path + ".gz";
    String pathWithHtml = path + ".html";
    
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path) || SPIFFS.exists(pathWithHtml)) // If the file exists, either as a compressed archive, or normal
    {
      if (SPIFFS.exists(pathWithGz)) // If there's a compressed version available
        path += ".gz"; // Use the compressed verion
  
      else if (SPIFFS.exists(pathWithHtml))
        path += ".html";

      String contentType = getContentType(path); // Get the MIME type
      request->send(SPIFFS, path, contentType, false, processor);
      Serial.println(String("\tSent file: ") + path);
    }
  });
}

String processor(const String& var)
{
  if (var == "IP") return WiFi.localIP().toString();
  else if (var == "GW") return WiFi.gatewayIP().toString();
  else if (var == "SUB") return WiFi.subnetMask().toString();
  else if (var == "MAC") return String(WiFi.macAddress());
  else if (var == "SSID") return String(ssid);
  else if (var == "PASSWORD") return String(password);
  else if (var == "SENSOR") return String(sensor_type);
  else if (var == "DHTPIN") return String(dhtPin);
  else if (var == "TEMPERATURE") return String(temperature);
  else if (var == "HUMIDITY") return String(humidity);
  else if (var == "HUM_OFFSET") return String(dhtHumOffset);
  else if (var == "TEMP_OFFSET") return String(dhtTempOffset);
  return String();
}
