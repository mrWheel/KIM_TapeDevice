
// ****************************************************************
// Sketch Esp8266 Filesystem Manager spezifisch sortiert Modular(Tab)
// created: Jens Fleischer, 2020-06-08
// last mod: Jens Fleischer, 2020-09-02
// For more information visit: https://fipsok.de/Esp8266-Webserver/littlefs-esp8266-270.tab
// ****************************************************************
// Hardware: Esp8266
// Software: Esp8266 Arduino Core 2.7.0 - 2.7.4
// Geprüft: von 1MB bis 2MB Flash
// Getestet auf: Nodemcu
/******************************************************************
  Copyright (c) 2020 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von LittleFS sollte als Tab eingebunden werden.
// #include <LittleFS.h> #include <ESP8266WebServer.h> müssen im Haupttab aufgerufen werden
// Die Funktionalität des ESP8266 Webservers ist erforderlich.
// "httpServer.onNotFound()" darf nicht im Setup des ESP8266 Webserver stehen.
// Die Funktion "setupFS();" muss im Setup aufgerufen werden.
/**************************************************************************************/

#include <list>
#include <tuple>

const char WARNING[] PROGMEM = R"(<h2>Check! Sketch is compiled with "FS:none"!)";
const char HELPER[]  PROGMEM = R"(
  <br>You first need to upload these two files:
  <ul>
    <li>littleFSexplorer.html</li>
    <li>littleFSexplorer.css</li>
  </ul>
  <hr>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="upload">
    <input type="submit" value="Upload">
  </form>
  <hr>
  <br/><b>or</b> you can use the <i>Flash Utility</i> to flash firmware or litleFS!
  <form action='/update' method='GET'>
    <input type='submit' name='SUBMIT' value='Flash Utility'/>
  </form>
)";


void setupFS() {                                                                       // Funktionsaufruf "setupFS();" muss im Setup eingebunden werden
  LittleFS.begin();
  httpServer.on("/format", formatFS);
  httpServer.on("/listFS", listFS);
  httpServer.on("/ReBoot", reBootESP);
  httpServer.on("/upload", HTTP_POST, sendResponce, handleUpload);
  httpServer.on("/update", updateFirmware);
  httpServer.onNotFound([]() 
  {
    if (!handleFile(httpServer.urlDecode(httpServer.uri())))
        httpServer.send(404, "text/plain", "FileNotFound");
  });
}

bool handleList() 
{                                           // Senden aller Daten an den Client
  FSInfo fs_info;  LittleFS.info(fs_info);  // Füllt FSInfo Struktur mit Informationen über das Dateisystem
  Dir dir = LittleFS.openDir("/");
  using namespace std;
  typedef tuple<String, String, int> records;
  list<records> dirList;
  while (dir.next()) 
  { 
    yield();  
    if (dir.isDirectory()) // Ordner und Dateien zur Liste hinzufügen
    {
      uint8_t ran {0};
      Dir fold = LittleFS.openDir(dir.fileName());
      while (fold.next())  
      {
        yield();
        ran++;
        dirList.emplace_back(dir.fileName(), fold.fileName(), fold.fileSize());
      }
      if (!ran) dirList.emplace_back(dir.fileName(), "", 0);
    }
    else {
      dirList.emplace_back("", dir.fileName(), dir.fileSize());
    }
  }
  dirList.sort([](const records & f, const records & l) 
  {                              // Dateien sortieren
    if (httpServer.arg(0) == "1") 
    {
      return get<2>(f) > get<2>(l);
    } else 
    {
      for (uint8_t i = 0; i < 31; i++) 
      {
        if (tolower(get<1>(f)[i]) < tolower(get<1>(l)[i])) return true;
        else if (tolower(get<1>(f)[i]) > tolower(get<1>(l)[i])) return false;
      }
      return false;
    }
  });
  dirList.sort([](const records & f, const records & l) 
  {                              // Ordner sortieren
    if (get<0>(f)[0] != 0x00 || get<0>(l)[0] != 0x00) 
    {
      for (uint8_t i = 0; i < 31; i++) 
      {
          if (tolower(get<0>(f)[i]) < tolower(get<0>(l)[i])) return true;
          else if (tolower(get<0>(f)[i]) > tolower(get<0>(l)[i])) return false;
      }
    }
    return false;
  });
  
  String temp = "[";
  for (auto& t : dirList) 
  {
    if (temp != "[") temp += ',';
    temp += "{\"folder\":\"" + get<0>(t) + "\",\"name\":\"" + get<1>(t) + "\",\"size\":\"" + formatBytes(get<2>(t)) + "\"}";
  }
  temp += ",{\"usedBytes\":\"" + formatBytes(fs_info.usedBytes) +                      // Berechnet den verwendeten Speicherplatz
          "\",\"totalBytes\":\"" + formatBytes(fs_info.totalBytes) +                   // Zeigt die Größe des Speichers
          "\",\"freeBytes\":\"" + (fs_info.totalBytes - fs_info.usedBytes) + "\"}]";   // Berechnet den freien Speicherplatz
  httpServer.send(200, "application/json", temp);
  return true;
  
} //  handleList() 

void deleteRecursive(const String &path) 
{
  actCatalog = false;
  if (LittleFS.remove(path)) 
  {
    LittleFS.open(path.substring(0, path.lastIndexOf('/')) + "/", "w");
    return;
  }
  Dir dir = LittleFS.openDir(path);
  while (dir.next()) 
  {
    yield();
    deleteRecursive(path + '/' + dir.fileName());
  }
  LittleFS.rmdir(path);
}

bool handleFile(String &&path) 
{
  actCatalog = false;
  if (httpServer.hasArg("new")) 
  {
    String folderName {httpServer.arg("new")};
    for (auto& c : {34, 37, 38, 47, 58, 59, 92}) for (auto& e : folderName) if (e == c) e = 95;    // Ersetzen der nicht erlaubten Zeichen
    LittleFS.mkdir(folderName);
  }
  if (httpServer.hasArg("sort")) return handleList();
  if (httpServer.hasArg("delete")) 
  {
    deleteRecursive(httpServer.arg("delete"));
    sendResponce();
    return true;
  }
  if (!LittleFS.exists("/littleFSexplorer.html")) 
  {
    httpServer.send(200, "text/html", LittleFS.begin() ? HELPER : WARNING);     // ermöglicht das hochladen der littleFSexplorer.html
  }
  if (path.endsWith("/")) path += "/index.html";
  if (path == "/LittleFS.html") sendResponce(); // Vorrübergehend für den Admin Tab
  return LittleFS.exists(path) ? ({File f = LittleFS.open(path, "r"); httpServer.streamFile(f, mime::getContentType(path)); f.close(); true;}) : false;
}

void handleUpload() 
{                            // Dateien ins Filesystem schreiben
  static File fsUploadFile;
  actCatalog = false;
  
  HTTPUpload& upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START) 
  {
    if (upload.filename.length() > 31) 
    {  // Dateinamen kürzen
      upload.filename = upload.filename.substring(upload.filename.length() - 31, upload.filename.length());
    }
    printf(PSTR("handleFileUpload Name: /%s\r\n"), upload.filename.c_str());
    fsUploadFile = LittleFS.open(httpServer.arg(0) + "/" + httpServer.urlDecode(upload.filename), "w");
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) 
  {
    printf(PSTR("handleFileUpload Data: %u\r\n"), upload.currentSize);
    fsUploadFile.write(upload.buf, upload.currentSize);
  } 
  else if (upload.status == UPLOAD_FILE_END) 
  {
    printf(PSTR("handleFileUpload Size: %u\r\n"), upload.totalSize);
    fsUploadFile.close();
    moveHexFilesFromRoot();
  }
}

void formatFS()      // Formatiert das Filesystem
{
  DebugTln("formatting littleFS ..");
  //LittleFS.format();
  sendResponce();
}

void listFS() 
{
  DebugTln("send littleFS data ..");
  sendResponce();
}


void sendResponce() 
{
  httpServer.sendHeader("Location", "/littleFSexplorer.html");
  httpServer.send(303, "message/http");
}

const String formatBytes(size_t const& bytes) 
{                                        // lesbare Anzeige der Speichergrößen
  return bytes < 1024 ? static_cast<String>(bytes) + " Byte" : bytes < 1048576 ? static_cast<String>(bytes / 1024.0) + " KB" : static_cast<String>(bytes / 1048576.0) + " MB";
}



//=====================================================================================
void updateFirmware()
{
#ifdef USE_UPDATE_SERVER
  DebugTln(F("Redirect to updateIndex .."));
  doRedirect("wait ... ", 1, "/updateIndex ", false);
#else
  doRedirect("UpdateServer not available", 10, "/", false);
#endif
      
} // updateFirmware()

//=====================================================================================
void reBootESP()
{
  DebugTln(F("Redirect and ReBoot .."));
  doRedirect("Reboot uTape ..", 30, "/", true);
      
} // reBootESP()

//=====================================================================================
void doRedirect(String msg, int wait, const char* URL, bool reboot)
{
  String redirectHTML = 
  "<!DOCTYPE HTML><html lang='en-US'>"
  "<head>"
  "<meta charset='UTF-8'>"
  "<style type='text/css'>"
  "body {background-color: lightblue;}"
  "</style>"
  "<title>Redirect to Main Program</title>"
  "</head>"
  "<body><h1>"+String(_HOSTNAME)+"</h1>"
  "<h3>"+msg+"</h3>"
  "<br><div style='width: 500px; position: relative; font-size: 25px;'>"
  "  <div style='float: left;'>Redirect over &nbsp;</div>"
  "  <div style='float: left;' id='counter'>"+String(wait)+"</div>"
  "  <div style='float: left;'>&nbsp; seconden ...</div>"
  "  <div style='float: right;'>&nbsp;</div>"
  "</div>"
  "<!-- Note: don't tell people to `click` the link, just tell them that it is a link. -->"
  "<br><br><hr>If you are not redirected automatically, click this <a href='/'>Main Program</a>."
  "  <script>"
  "      setInterval(function() {"
  "          var div = document.querySelector('#counter');"
  "          var count = div.textContent * 1 - 1;"
  "          div.textContent = count;"
  "          if (count <= 0) {"
  "              window.location.replace('"+String(URL)+"'); "
  "          } "
  "      }, 1000); "
  "  </script> "
  "</body></html>\r\n";
  
  DebugTln(msg);
  httpServer.send(200, "text/html", redirectHTML);
  if (reboot) 
  {
    delay(5000);
    ESP.restart();
    delay(5000);
  }
  
} // doRedirect()
