
void Start_Server() // Start a HTTP server with a file read handler and an upload handler
{ 
  server.on("/style.css", [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/SubmitOn", [] (AsyncWebServerRequest *request) {
    Serial.println("Submit ON");
    
    saveInputPinState = !saveInputPinState;
    
    request->redirect("/Info.html");
  });

  server.on("/SubmitOff", [] (AsyncWebServerRequest *request) {
    Serial.println("Submit OFF");

    saveInputPinState = !saveInputPinState;
    
    request->redirect("/Info.html");
  });
  
  server.on("/SubmitReset", [] (AsyncWebServerRequest *request) {
    Serial.println("Resetting ESP");
    request->redirect("/SuccessReset.html");
    resetESP = true;
  });
  
  server.on("/SubmitRipr", [] (AsyncWebServerRequest *request) {
    Serial.println("Riprisino Submit");
    if(deleteFile(SPIFFS, "/configSSID.txt") && deleteFile(SPIFFS, "/configPassword.txt") && deleteFile(SPIFFS, "/configDevice.txt"))
      request->send(SPIFFS, "/SuccessRipr.html", "text/html");
  });

  server.on("/submit", HTTP_POST, [] (AsyncWebServerRequest *request) {
    Serial.println("Config Submit");
    int params = request->params();
    for(int i=0;i<params;i++)
    {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()) Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size()); 
      else if(p->isPost()) Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      else Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());

      if(p->name() == "ssid") 
      {
        request->arg("ssid").toCharArray(ssid, 40);
        writeFile(SPIFFS, "/configSSID.txt", ssid);
      }
      
      else if(p->name() == "pw") 
      {
        request->arg("pw").toCharArray(password, 40);
        writeFile(SPIFFS, "/configPassword.txt", password);
      }
      
      else if(p->name() == "name") 
      {
        request->arg("name").toCharArray(Device_Name, 40);
        writeFile(SPIFFS, "/configDevice.txt", Device_Name);
      }
      
      else if(p->name() == "input") 
      {
        inputPin = request->arg("input").toInt();
        writeFile(SPIFFS, "/configInput.txt", String(inputPin).c_str());
      }
      
      else if(p->name() == "output") 
      {
        outputPin = request->arg("output").toInt();
        writeFile(SPIFFS, "/configOutput.txt", String(outputPin).c_str());
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
    
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path) || SPIFFS.exists(pathWithHtml)) // If the file exists, either as a compressed archive, or normal
    {
      if (SPIFFS.exists(pathWithGz)) // If there's a compressed version available
        path += ".gz"; // Use the compressed verion
  
      else if (SPIFFS.exists(pathWithHtml))
        path += ".html";

      String contentType = getContentType(path); // Get the MIME type
      request->send(SPIFFS, path, contentType, false, processor);
      Serial.println(String("\tSent file: ") + path);
    }
    else if(!espalexa.handleAlexaApiCall(request)) //if you don't know the URI, ask espalexa whether it is an Alexa control request
    {
      //whatever you want to do with 404s
      request->send(404, "text/plain", "Not found");
    }
  });
}

String processor(const String& var)
{
  if(var == "OUTSTATUS") return String(saveOutputPinState);
  else if(var == "IP") return WiFi.localIP().toString();
  else if(var == "GW") return WiFi.gatewayIP().toString();
  else if(var == "SUB") return WiFi.subnetMask().toString();
  else if(var == "MAC") return String(WiFi.macAddress());
  else if(var == "SSID") return String(ssid);
  else if(var == "PASSWORD") return String(password);
  else if(var == "NAME") return String(Device_Name);
  else if(var == "INPUT") return String(inputPin);
  else if(var == "OUTPUT") return String(outputPin);
  return String();
}
