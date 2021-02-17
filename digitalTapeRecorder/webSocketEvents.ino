/*
***************************************************************************  
**  Program  : digitalTapeRecorder v2.0 / Record
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/


//===========================================================================================
//-- everytime a webSocketEvent occurs this function is processed
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
                readProgDetailsByID(actFileID);
                //sendExternalFile("txt");
                printMenu();
                DebugTln("Done!");

              } else if (text.indexOf("REC") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                recordTape();
                printMenu();
                DebugTln("Done!");
                
              } else if (text.indexOf("PLAY") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                playbackTape();
                printMenu();
                DebugTln("Done!");
                
              } else if (text.indexOf("FFWRD") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                actFileID = findNextID(actFileID);
                scrollInterval = millis();
                readProgDetailsByID(actFileID);
                //sendExternalFile("txt");
                printMenu();
                DebugTln("Done!");
                
              } else if (text.indexOf("EMPTY") > -1) 
              {
                DebugTln(text);
                webSocket.broadcastTXT("blockButtons");
                actFileID = findFirstEmptyID();
                scrollInterval = millis();
                //-readProgDetailsByID(actFileID);
                printMenu();
                DebugTln("Done!");
              }
            }
            if (text.startsWith("newName")) 
            {
              snprintf(newName, sizeof(newName), "%s", String(text).substring(8).c_str());
              DebugTf("received newName[%s]\r\n", newName);
              renameFilesByID(progDetails.numID, newName);
              readProgDetailsByID(actFileID);
              printMenu();
            }
            if (text.startsWith("newDESC")) 
            {
              snprintf(newName, sizeof(newName), "%s", String(text).substring(8).c_str());
              DebugTf("received newDESC\r\n%s\r\n", String(text).substring(8).c_str());
              //--- only send text from pos 8 to the end (skip "newDESC:")--
              writeExternalFile(actFileID, "txt", String(text).substring(8).c_str());
              printMenu();
            }
            if (text.startsWith("newPTP")) 
            {
              snprintf(newName, sizeof(newName), "%s", String(text).substring(7).c_str());
              DebugTf("received newDESC\r\n%s\r\n", String(text).substring(7).c_str());
              //--- only send text from pos 7 to the end (skip "newPTP:") --
              writeExternalFile(actFileID, "ptp", String(text).substring(7).c_str());
              printMenu();
            }
            if (text.startsWith("newASM")) 
            {
              snprintf(newName, sizeof(newName), "%s", String(text).substring(7).c_str());
              DebugTf("received newASM\r\n%s\r\n", String(text).substring(7).c_str());
              //--- only send text from pos 7 to the end (skip "newASM:") --
              writeExternalFile(actFileID, "asm", String(text).substring(7));
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
              //Debugf("websocket.send(%s)\r\n", jsonString.c_str());
              webSocket.broadcastTXT(jsonString);
              sendProgDetails();
            } 

            if (text.indexOf("getDESC") > -1) 
            {
              DebugTln(text);
              sendExternalFile("txt");            
            }           
            if (text.indexOf("getPTP") > -1) 
            {
              DebugTln(text);
              sendExternalFile("ptp");            
            }             
            if (text.indexOf("getASM") > -1) 
            {
              DebugTln(text);
              sendExternalFile("asm");            
            }             
            if (text.indexOf("getCatalog") > -1) 
            {
              DebugTln(text);
              int savFileID = actFileID;
              listProgramFiles("ws");
              actFileID = savFileID;
              readProgDetailsByID(actFileID);
              printMenu();
            }             
            if (text.indexOf("setID-actFileID") > -1) 
            {
              DebugTln(text); // setID-XX
              readProgDetailsByID(actFileID);
              printMenu();
            }             
            else if (text.indexOf("setID") > -1) 
            {
              DebugTln(text); // setID-XX
              sprintf(progDetails.ID, "%s", text.substring(6).c_str());
              actFileID = (int)strtol(progDetails.ID, NULL, 16);
              Debugf("setID-[%s] -> actFileID[%d]\r\n", progDetails.ID, actFileID);
              readProgDetailsByID(actFileID);
              printMenu();
            }             
            
            break;
                        
  } // switch(type)
    
} // webSocketEvent()


//===========================================================================================
//-- send the information in the progDetails struct to
//-- the web client (JSON format)
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
  //Debugf("\r\nbroadcastTXT(%s)\r\n",jsonString.c_str());
  webSocket.broadcastTXT(jsonString.c_str());

} // sendProgDetails()


//===========================================================================================
//-- send the information in an external file (".ptp", ".asm" enz.)
//-- to the web client (JSON format)
//===========================================================================================
void sendExternalFile(const char *fileType)
{
  char cLine[100];

  DebugTf("proces filetype [%s] ...", fileType);
  String jsonString  = "{";
  if (strncmp(fileType, "txt", 3) == 0)
  {
    Debugf(" yes! (txt)\r\n");
    jsonString += "\"msgType\":\"progDESC\",\"Text\":\"";
    snprintf(filePath, sizeof(filePath), "/%s/%s.txt", progDetails.ID, progDetails.Name);
  }
  else if (strncmp(fileType, "ptp", 3) == 0)
  {
    Debugf(" yes! (ptp)\r\n");
    jsonString += "\"msgType\":\"progPTP\",\"Text\":\"";
    snprintf(filePath, sizeof(filePath), "/%s/%s.ptp", progDetails.ID, progDetails.Name);
  }
  else if (strncmp(fileType, "asm", 3) == 0)
  {
    Debugf(" yes! (asm)\r\n");
    jsonString += "\"msgType\":\"progASM\",\"Text\":\"";
    snprintf(filePath, sizeof(filePath), "/%s/%s.asm", progDetails.ID, progDetails.Name);
  }
  else
  {
    Debugf("Don't know what [%s] is! Abort\r\n", fileType);
    return;
  }

  DebugTf("Try to open [%s] ..", filePath);
  
  File D = LittleFS.open(filePath, "r");
  if (!D)
  {
    Debugln(" not found!\r");
    jsonString += " \"}";
    //Debugf("\r\nbroadcastTXT(%s)\r\n",jsonString.c_str());
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
  //Debugf("\r\nbroadcastTXT(%s)\r\n",jsonString.c_str());
  webSocket.broadcastTXT(jsonString.c_str());

} // sendExternalFile()


//===========================================================================================
//-- send a Status message to the web client (JSON format)
//===========================================================================================
void sendStatusMessage(const char *Msg)
{
  String jsonString  = "{";
  jsonString += "\"msgType\":\"statusMsg\"";
  jsonString += ", \"message\":\""+String(Msg)+"\"";
  jsonString += "}";
  webSocket.broadcastTXT(jsonString.c_str());

} // sendStatusMessage()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/
