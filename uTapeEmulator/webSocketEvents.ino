

//===========================================================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) 
{
    String  text = String((char *) &payload[0]);
    char *  textC = (char *) &payload[0];
    String  pOut[5], pDev[5], pVal[5], words[10], jsonString;
    int8_t  deviceNr;

    switch(type) 
    {
        case WStype_DISCONNECTED:
            DebugTf("[%u] Disconnected!\r\n", num);
            isConnected = false;
            break;
            
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                if (!isConnected) 
                {
                  DebugTf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                  isConnected = true;
                  webSocket.broadcastTXT("{\"msgType\":\"ConnectState\",\"Value\":\"Connected\"}");
                  //sendDevInfo();
                }
        
            }
            break;
            
        case WStype_TEXT:

            DebugTf("[%u] Got message: [%s]\r\n", num, payload);
            splitString(text, ':', words, 10);
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            if (text.startsWith("Button")) 
            {
              if (text.indexOf("FREV") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                actFileID = findPreviousID(actFileID);
                scrollInterval = millis();
                printMenu();
                webSocket.broadcastTXT("freeButtons");
                DebugTln("Done!");

              } else if (text.indexOf("REC") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                recordTape();
                printMenu();
                webSocket.broadcastTXT("freeButtons");
                DebugTln("Done!");
                
              } else if (text.indexOf("PLAY") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                playbackTape();
                printMenu();
                webSocket.broadcastTXT("freeButtons");
                DebugTln("Done!");
                
              } else if (text.indexOf("FFWRD") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                actFileID = findNextID(actFileID);
                scrollInterval = millis();
                printMenu();
                webSocket.broadcastTXT("freeButtons");
                DebugTln("Done!");
                
              } else if (text.indexOf("EMPTY") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                actFileID = findFirstEmptyID();
                scrollInterval = millis();
                printMenu();
                webSocket.broadcastTXT("freeButtons");
                DebugTln("Done!");
              }
            }
            if (text.startsWith("newName")) 
            {
              snprintf(newName, sizeof(newName), "%s", String(text).substring(8).c_str());
              DebugTf("received newName[%s]\r\n", newName);
              renameProgName(progDetails.numID, newName);
              readProgDetailsByID(actFileID);
              printMenu();
            }
            if (text.startsWith("newDesc")) 
            {
              snprintf(newName, sizeof(newName), "%s", String(text).substring(8).c_str());
              DebugTf("received newDesc\r\n%s\r\n", String(text).substring(8).c_str());
              writeDescription(actFileID, String(text).substring(8));
              printMenu();
            }
            if (text.startsWith("newLock")) 
            {
              DebugTf("received newLock[%s]\r\n", String(text).substring(8).c_str());
              if ((String(text).substring(8) == "true") || (String(text).substring(8) == "on"))
              {
                DebugTln("Set Lock to TRUE");
                writeLockFile(actFileID, true);
              }
              else
              {
                DebugTln("Set Lock to FALSE");
                writeLockFile(actFileID, false);
              }
              printMenu();
            }

            if (text.indexOf("getDevInfo") > -1) 
            {
              DebugTln(text);
              jsonString  = "{";
              jsonString += "\"msgType\": \"devInfo\"";
              jsonString += ", \"devName\": \"" + String(_HOSTNAME) + " \"";
              jsonString += ", \"devIPaddress\": \"" + WiFi.localIP().toString() + " \"";
              jsonString += ", \"devVersion\": \" [" + String(_FW_VERSION) + "]\"";
              jsonString += "}";
              Debugf("websocket.send(%s)\r\n", jsonString.c_str());
              webSocket.broadcastTXT(jsonString);
              sendProgDetails();
              sendProgDescription();              
            } 
            break;
                        
    } // switch(type)
    
} // webSocketEvent()


//===========================================================================================
void sendProgDetails()
{
  String jsonString  = "{";
  jsonString += "\"msgType\":\"progDetails\"";
  jsonString += ", \"ID\":\""+String(progDetails.ID)+"\"";
  jsonString += ", \"Strt\":\""+String(progDetails.Strt)+"\"";
  if (progDetails.Size > 1024)
        jsonString += ", \"Size\":\""+String(progDetails.Size/1024)+" kB\"";
  else  jsonString += ", \"Size\":\""+String(progDetails.Size)+" Bytes\"";
  jsonString += ", \"End\":\""+String(progDetails.End)+"\"";
  jsonString += ", \"Lock\":";
  if (progDetails.Lock) jsonString += "\"1\"";
  else                  jsonString += "\"0\"";
  jsonString += ", \"Name\":\""+String(progDetails.Name)+"\"";
  jsonString += "}";
  Debugf("\r\nbroadcastTXT(%s)\r\n",jsonString.c_str());
  webSocket.broadcastTXT(jsonString.c_str());

} // sendProgDetails()



//===========================================================================================
void sendProgDescription()
{
  char cLine[100];
  String jsonString  = "{";
  jsonString += "\"msgType\":\"progDescription\",\"Text\":\"";

  snprintf(filePath, sizeof(filePath), "/%s/%s.txt", progDetails.ID, progDetails.Name);
  Debugf("Try to open [%s] ..", filePath);
  
  File D = LittleFS.open(filePath, "r");
  if (!D)
  {
    Debugln(" not found!\r");
    jsonString += " \"}";
    Debugf("\r\nbroadcastTXT(%s)\r\n",jsonString.c_str());
    webSocket.broadcastTXT(jsonString.c_str());
    return;
  }
  Debugln(" OK!\r");
  
  while (D.available())
  {
    memset(cLine, 0, sizeof(cLine));
    D.readBytesUntil('\n', cLine, 66);
    for(int i=0; i<strlen(cLine); i++)
    {
      if (cLine[i] == '\r') cLine[i] = ' ';
      if (cLine[i] == 34)   cLine[i] = 96;  // single quote to double quote?
      if (cLine[i] < ' ')   cLine[i] = ' ';
    }
    jsonString.concat(String(cLine));
    //if (strlen(cLine) < 66) jsonString.concat("@n$");
    if (strlen(cLine) < 66) jsonString.concat("\\n");
  }
  D.close();
  jsonString += "\\n\"}";
  Debugf("\r\nbroadcastTXT(%s)\r\n",jsonString.c_str());
  webSocket.broadcastTXT(jsonString.c_str());

} // sendProgDescription()

/*EOF*/
