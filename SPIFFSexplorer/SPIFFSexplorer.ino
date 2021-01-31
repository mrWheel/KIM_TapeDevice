/*
**  Utility  : SPIFFSexplorer 
**   
**  The purpose of this program is to upload programs that you saved
**  on the Solid State Tape Device (uTape) to your computer.
**  
**  The pré 2.0 version of the uTape did not have this functionality.
**  
**  Point your browser to the correct IP address (see Serial Monitor during
**  startup) or to "SPIFFSexplorer.local"
**
**  Compile this program with the correct setting of the memoty-model
**  otherwise it will not be able to read SPIFFS.
**  You either compiled your version with
**  
**  - Flash size: "4M (3M SPIFFS)" 
**  - Flash size: "4M (2M SPIFFS)"  
**  
**  You should compile this program with the same setting!
**  
**  If you don't it will only show an empty files list and you can
**  compile it again with an other Flash size until you can see
**  al your program's.
**  
**  DO NOT UPLOAD A DATA MAP TO YOUR DEVICE OR YOU WILL LOSE 
**  ALL FILES ON IT WITH NO WAY TO RECOVER!!
*/
#define _FW_VERSION "v0.0.0 (31-01-2021)"
/*
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
  Arduino-IDE settings for this program:

    - Board: "Generic ESP8266 Module"
    - Flash mode: "DIO" | "DOUT"    
    - Flash size: "4M (3M SPIFFS)" => or how you compiled your version  
    - DebugT port: "Disabled"
    - DebugT Level: "None"
    - IwIP Variant: "v2 Lower Memory"
    - Reset Method: "none"   // but will depend on the programmer!
    - Crystal Frequency: "26 MHz" 
    - VTables: "Flash"
    - Flash Frequency: "40MHz"
    - CPU Frequency: "80 MHz"
    - Buildin Led: "2"  // 
    - Upload Speed: "115200"    => what-ever you're confortable with                                                                                                                                                                                                                                             
    - Erase Flash: "Only Sketch"
    - Port: <select correct port>
*/
#define _HOSTNAME "SPIFFSexplorer"

/******************** don't change anything below this comment **********************/

//#include <TimeLib.h>            //  https://github.com/PaulStoffregen/Time
#include <TelnetStream.h>
#include "Debug.h"
#include "networkStuff.h"
#include "indexPage.h"
#include <FS.h>                 // part of ESP8266 Core https://github.com/esp8266/Arduino

char      cMsg[150], fChar[10];
String    pTimestamp;
int8_t    prevNtpHour = 0;
uint64_t  upTimeSeconds;


//===========================================================================================
void handleIndexPage() 
{
  DebugT("now in handleIndexPage() ..");
  //String indexHtml = serverIndex;
  //httpServer.send(200, "text/html", indexHtml);
  httpServer.send(200, "text/html", serverIndex);
  Debugln(" .. and I'm out again..");
  
} //  handleIndexPage() 


//===========================================================================================
void setup() 
{
  Serial.begin(19200);
  Debugln("\nBooting ... \n");

  pinMode(LED_BUILTIN, OUTPUT);

  startWiFi();
  startTelnet();
   
  Debug("Gebruik 'telnet ");
  Debug(WiFi.localIP());
  Debugln("' voor verdere debugging");

  startMDNS(_HOSTNAME);

  for (int I = 0; I < 10; I++) 
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(100);
  }
//================ SPIFFS ===========================================
  if (!SPIFFS.begin()) 
  {
    DebugTln("SPIFFS Mount failed\r");   // Serious problem with SPIFFS 
    
  } 
  else 
  { 
    DebugTln("SPIFFS Mount succesfull\r");
  }
//=============end SPIFFS =========================================

  httpServer.on("/",           HTTP_GET, handleIndexPage);
  httpServer.on("/index.html", HTTP_GET, handleIndexPage);
  httpServer.serveStatic("/FSexplorer.png",   SPIFFS, "/FSexplorer.png");

  httpServer.on("/ReBoot", HTTP_POST, handleReBoot);

  httpServer.on("/FSexplorer", HTTP_POST, handleFileDelete);
  httpServer.on("/FSexplorer", handleRoot);
  httpServer.on("/FSexplorer/upload", HTTP_POST, []() {
    httpServer.send(200, "text/plain", "");
  }, handleFileUpload);

  httpServer.onNotFound([]() {
      DebugTf("onNotFound(%s)\r\n", httpServer.uri().c_str());
      if (httpServer.uri() == "/" || httpServer.uri() == "/index.html") 
      {
        handleIndexPage();
      }
      if (!handleFileRead(httpServer.uri())) 
      {
        httpServer.send(404, "text/plain", "FileNotFound");
      }
  });

  httpServer.begin();
  DebugTln( "HTTP server gestart\r" );
  DebugTln("Startup complete! \r\n");  

} // setup()


void loop() 
{
  httpServer.handleClient();
  MDNS.update();

} // loop()
