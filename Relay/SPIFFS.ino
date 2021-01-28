
void SPIFFS_Setup()
{
  SPIFFS.begin(); // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) // List the file system contents
    { 
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

String formatBytes(size_t bytes) // convert sizes in bytes to KB and MB
{ 
  if (bytes < 1024) 
  {
    return String(bytes) + "B";
  } 
  
  else if (bytes < (1024 * 1024)) 
  {
    return String(bytes / 1024.0) + "KB";
  } 
  
  else if (bytes < (1024 * 1024 * 1024)) 
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

String getContentType(String filename) // determine the filetype of a given filename, based on the extension
{ 
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing ' %s ' in file: %s\r\n", message, path);
  File file = fs.open(path, "w");
  if(!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)) Serial.println("- file written");
  else Serial.println("- write failed");
}

String readFile(fs::FS &fs, const char * path)
{
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory())
  {
    Serial.println("- empty file or failed to open file");
    return String(" ");
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available())
  {
    fileContent += String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

boolean deleteFile(fs::FS &fs, const char * path)
{
  Serial.printf("Deleting file: %s\r\n", path);

  if(!fs.exists(path))
  {
    Serial.println("File NOT exist!");
    return true;
  }
  if(!fs.remove(path))
  {
    Serial.println("File NOT removed!");
    return false;
  }
  Serial.println("File removed!");
  return true;
}
