/*
***************************************************************************  
 *  Program  : uTapeEmulator
 *  Copyright (c) 2021 Willem Aandewiel
 */
#define _FW_VERSION "v2.0.0 WS (31-01-2021)"
/* 
*  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************  
 * Board:         "Generic ESP8266 Module"
 * Builtin Led:   "2"
 * CPU Frequency: "80 MHz" or "160 MHz"
 * Flash Size:    "4MB (FS:3MB OTA:~512KB)"
***************************************************************************/  

#define USE_UPDATE_SERVER
#define DEBUG_ON
#define _HAS_BUTTONS    false    // false=no buttons, true=yes we have buttons

//---------- no need to change enything after this ------

#define _HOSTNAME   "uTape"

#include <TelnetStream.h>
#include "Debug.h"
#include "networkStuff.h"
#include "LittleFS.h"

#include <SSD1306.h>    // https://github.com/ThingPulse/esp8266-oled-ssd1306

//#define _BV(bit)            (1 << (bit))
#define _SET(a,b)           ((a) |= _BV(b))
#define _CLEAR(a,b)         ((a) &= ~_BV(b))

#define _LED_PIN        LED_BUILTIN

#define _SDA            4       // D2   - GPIO04
#define _SCL            5       // D1   - GPIO05

#define _PLL_RESET      2       // D4   - GPIO02
#define _PLL_IN_PIN     13      // D7   - GPIO13  // swaped 27-01
#define _TAPE_OUT_PIN   12      // D6   - GPIO12  // swaped 27-01

#define _PLAY_BUTTON    0       // A0
#define _FREV_BUTTON    14      // D5
#define _FFWD_BUTTON    16      // D0

#define _DIMOLEDTIME    300000  // five minutes
#define _BUTTONTIME         50
#define _LOWSCROLLTIME     500
#define _HIGHSCROLLTIME   3000

#define DISPLAY_WIDTH      128
#define DISPLAY_HEIGHT      64

#define NAME_SIZE        23 // /<ID>+/+<fName>+".hex" => 4+23+4 = 31
#define MAX_PROG_SIZE    4096

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, _SDA, _SCL);

extern void dspl_Init();

struct { uint8_t  numID; 
         char     ID[6];
         char     Strt[5];
         char     End[5];
         char     Name[NAME_SIZE];
         int      Size;
         bool     Lock; 
       } progDetails;

char            ascii[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

char            wsSend[100];          // String to send via WebSocket
bool            catalog[256];
bool            actCatalog = false;
int16_t         progCounter;
uint16_t        progPos;

uint32_t        getTimingDuration, dimOledTimer, processKeyTimer, processButtonTimer;
uint32_t        bitStartTime, bitTime;
byte            actByte;
int16_t         actFileID = 0, savFileID;
char            fileID[5];
char            idDir[6];
char            filePath[33];
char            newName[NAME_SIZE];
char            cBuff[81], sBuff[30], scrollName[50];
char            lByte, hByte;
int16_t         tmpB, idxName;
uint32_t        scrollInterval;
char            buttonChoice, mChoice;
bool            displayIsOff;
char            cIn = 0;
bool            buttonsActive = false;


//------------------------------------------------------
// SPrints out Binary value (1 or 0) of byte
//------------------------------------------------------
void printByte(int c) {
    if (!doVerbose) return;
    for (int bit = 7; bit >= 0; bit--) {
        // Compare bit 7-0 in byte
        if (c & (1 << bit)) {
            SPrint ("1");
        } else {
            SPrint ("0");
        }
    }
}   // printByte()


//------------------------------------------------------
bool buttonChanged() 
{
    uint32_t  waitTime;
    int16_t   aVal;

#if _HAS_BUTTONS == false
     return false;
#endif
    
    aVal = analogRead(_PLAY_BUTTON);
    if (aVal > 800) 
    {
        delay(100);
        aVal = analogRead(_PLAY_BUTTON);
        if (aVal > 800) 
        {
            SPrintf("Record button! [%d]\r\n", aVal);
            buttonChoice = 'R';
            while (analogRead(_PLAY_BUTTON) > 800) 
            { 
              yield(); 
              httpServer.handleClient();
            }
            return true;
            
        } else return false;
    } 
    else 
    {
        if (aVal > 300 && aVal < 800) {
            delay(100);
            aVal = analogRead(_PLAY_BUTTON);
            if (aVal > 300 && aVal < 800) {
                SPrintf("Play button! [%d]\r\n", aVal);
                buttonChoice = 'P';
                while (analogRead(_PLAY_BUTTON) > 300) 
                { 
                  yield(); 
                  httpServer.handleClient();
                }
                return true;
                
            } else return false;
        } 
    }
    if (digitalRead(_FFWD_BUTTON) == LOW) 
    {
        delay(100);
        if (digitalRead(_FFWD_BUTTON) == LOW)
        {
            SPrintln("FastForward button!");
            buttonChoice = '+';
            waitTime = millis();
            while (   (digitalRead(_FFWD_BUTTON) == LOW)
                   && ((millis() - waitTime) < 1000) ) 
            { 
              yield(); 
              httpServer.handleClient();
            }
            return true;
        }
    }
    if (digitalRead(_FREV_BUTTON) == LOW) 
    {
        delay(100);
        if (digitalRead(_FREV_BUTTON) == LOW) 
        {
            SPrintln("FastReverse button!");
            buttonChoice = '-';
            while (digitalRead(_FREV_BUTTON) == LOW)
            { 
              yield(); 
              httpServer.handleClient();
            }
            return true;
        }
    }

    return false;

}   // buttonChanged()


//------------------------------------------------------
// Decode fileName "<text>_<hexStart>.hex ==> <text> and <start>
//------------------------------------------------------
void fileName2Name(uint8_t id, const char *fName) 
{
  progDetails.numID = id;
  sprintf(progDetails.ID, "%02x", id);
  arrayToUpper(progDetails.ID, strlen(progDetails.ID));
  memset(progDetails.Name, 0, sizeof(progDetails.Name));
  snprintf(cBuff, sizeof(cBuff), "%s", fName);
  //---- remove the ".hex" part...
  cBuff[strlen(cBuff)-4] = 0;
  snprintf(progDetails.Name, sizeof(progDetails.Name), "%s", cBuff);
  DebugTf("ID[%s], Name[%s]\r\n", progDetails.ID, progDetails.Name);
   
} //  fileName2Name()


//------------------------------------------------------
char read1Nibble(File fIn) 
{
  while (true && fIn.available())
  {
    char nibbleIn = fIn.read();    // skip ID lnibble
    if (isHex((char)nibbleIn))
    {
       return nibbleIn;
    }
  } 
  
} // read1Nibble()


//------------------------------------------------------
void readCatalog() 
{
  int  slotNr;
  char dirName[5];
  
  Dir dir = LittleFS.openDir("/");

  SPrintln("in readCatalog() ..");

  for (slotNr=1; slotNr<=255; slotNr++) catalog[slotNr] = false;
    
  while (dir.next()) 
  {
    yield();  
    if (dir.isDirectory()) // it's a map!!
    {
      snprintf(dirName, sizeof(dirName), "%s", dir.fileName().c_str());
      if ((strlen(dirName) == 2) && isHex(dirName[0]) && isHex(dirName[1]))
      {
        sscanf(dirName, "%x", &slotNr);
        if (slotNr >= 0 && slotNr <= 255)
        {
          //Debugf("Dir name[%s] -> dec[%d]\r\n", dirName, slotNr);
          catalog[slotNr] = true;
        }
      }
    }
  } // while next..  
  
  actCatalog = true;
  
} // readCatalog();


//------------------------------------------------------
int getProgramSize(const char *fID, const char *fName) 
{
  File F;
  char filePath[50], bIn;
  int  bCount;
  
  snprintf(filePath, sizeof(filePath), "/%s/%s.hex", fID, fName);
  F = LittleFS.open(filePath, "r");
  if (!F)
  {
    SPrintf("Error opening [%s]\r\n", filePath);
    return 0;
  }
  bCount = -6;  // skip first 6 bytes (ID, SAL, SAH)
  while (F.available())
  {
    bIn = F.read();
    if (bIn == '/') break;
    if (   (bIn >= '0' && bIn <= '9')
        || (bIn >= 'A' && bIn <= 'F')
       ) bCount++;
  }
  F.close();

  return ((bCount / 2)-1);
  
} // getProgramSize()


//------------------------------------------------------
void moveHexFilesFromRoot() 
{
  char baseName[NAME_SIZE];
  
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) 
  {
    yield();
    if (dir.fileName().endsWith(".hex"))
    {
      actFileID = findFirstEmptyID();
      DebugTf("create idDir for [%s] ..\r\n", dir.fileName().c_str());
      sprintf(idDir, "/%02x", actFileID);
      toupper(idDir[0]);
      toupper(idDir[1]);
      displayMsgAndWait(String(idDir), 1) ;
      DebugTf("idDir -> [%s]\r\n", idDir);
      snprintf(baseName, sizeof(baseName), "%s", dir.fileName().c_str());
      //DebugTf("(1) baseName[%s]\r\n", baseName);
      baseName[strlen(baseName)-4] = 0;
      displayMsgAndWait(String(baseName), 2) ;
      //DebugTf("(2) baseName[%s]\r\n", baseName);
      //-- baseNaem.hex ---
      snprintf(cBuff, (NAME_SIZE), "%s.hex", baseName);
      DebugTf("(1) cBuff[%s]\r\n", cBuff);
      sprintf(filePath, "%s/%s", idDir, cBuff);
      DebugTf("rename(%s -> %s)\r\n", cBuff, filePath);
      LittleFS.mkdir(idDir);
      LittleFS.rename(cBuff, filePath);
      //-- baseNaem.txt ---
      snprintf(cBuff, (NAME_SIZE), "%s.txt", baseName);
      DebugTf("(2) cBuff[%s]\r\n", cBuff);
      sprintf(filePath, "%s/%s", idDir, cBuff);
      DebugTf("rename(%s -> %s)\r\n", cBuff, filePath);
      LittleFS.rename(cBuff, filePath);
    }
  }
  turnOnOledDisplay();
  readProgDetailsByID(actFileID);
  displayProgDetails();
  
} //  moveHexFilesFromRoot()


//------------------------------------------------------
void writeLockFile(uint16_t numID, bool newState)
{
  DebugTf("change state [%s.lck] to [%s]\r\n", progDetails.Name, newState? "Lock":"R/W");
  
  if (numID == 0 || numID >= 0xE0) 
  {
      SPrintln("\r\nError: cannot alter Lock of this program\r\n");
      return;
  }
  readProgDetailsByID(numID);
  progDetails.Lock = newState;
  snprintf(filePath, sizeof(filePath), "/%s/%s.lck", progDetails.ID, progDetails.Name);
  if (newState)
  {
    DebugTf("open(%s, \"w\") ..", filePath);
    File fileData = LittleFS.open(filePath, "w");
    if (fileData)
    {
      fileData.write("1");
      fileData.close();
      Debugln("OK");
    }
    else
    {
      Debugln("Error opening file!");
    }
  }
  else
  {
    DebugTf("remove(%s)\r\n", filePath);
    LittleFS.remove(filePath);
  }
  
} //  writeLockFile()


//------------------------------------------------------
void writeDescription(uint8_t numID, String data) 
{
  DebugTf("write to [%s.txt]\r\n", progDetails.Name);
  
  if (numID == 0 || numID >= 0xE0) 
  {
      SPrintln("\r\nError: cannot alter description of this program\r\n");
      return;
  }
  readProgDetailsByID(numID);

  snprintf(filePath, sizeof(filePath), "/%s/%s.txt", progDetails.ID, progDetails.Name);
  DebugTf("open(%s, \"w\")\r\n", filePath);
  File fileData = LittleFS.open(filePath, "w");
  for(int p=0; p<data.length(); p++)
  {
    fileData.write(data[p]);
  }
  fileData.close();

} //  writeDescription()


//------------------------------------------------------
bool readProgDetailsByID(uint8_t fileID) 
{
    File lckFile;
    uint8_t lenName, nibbleCnt;
    String space = "                           ";
    uint8_t nibbleIn;
    
    Dir dir;
    sprintf(progDetails.ID, "%02x", fileID);
    arrayToUpper(progDetails.ID, strlen(progDetails.ID));
    sprintf(idDir, "/%02x", fileID);
    arrayToUpper(idDir, strlen(idDir));
    //DebugTf("check id[%s] ..\r\n", progDetails.ID);      
    dir = LittleFS.openDir(idDir);
    while (dir.next()) 
    {
      yield();
      DebugTf("Processing [%s] ..\r\n", dir.fileName().c_str());
      if (dir.fileName().endsWith(".lck")) continue;
      if (dir.fileName().endsWith(".hex")) 
      {
        fileName2Name(fileID, dir.fileName().c_str());
        progDetails.Size = getProgramSize(progDetails.ID, progDetails.Name);
        DebugTf("program[%s] => [%d bytes]\r\n", progDetails.Name, progDetails.Size);      
        DebugTf("progSize[%d]\r\n", progDetails.Size);

        snprintf(filePath, sizeof(filePath), "%s/%s", idDir, dir.fileName().c_str());
        DebugTf("Checking [%s] ..\r\n", filePath);

        File fileData = LittleFS.open(filePath, "r");
        if (fileData)
        {
          nibbleIn = read1Nibble(fileData);
          nibbleIn = read1Nibble(fileData);
          progDetails.Strt[2] = read1Nibble(fileData);
          progDetails.Strt[3] = read1Nibble(fileData);
          progDetails.Strt[0] = read1Nibble(fileData);
          progDetails.Strt[1] = read1Nibble(fileData);
          progDetails.Strt[4] = 0;
          DebugTf("=>[Strt]>[$%s]\r\n", progDetails.Strt);
          fileData.close();
          DebugTf("=>[Size]>[$%04x]\r\n", progDetails.Size);
          int tmp = strtol(progDetails.Strt, NULL, 16);
          DebugTf("=>>[End]>[$%04x]\r\n", (tmp + progDetails.Size));
          snprintf(progDetails.End, sizeof(progDetails.End), "%04x", (tmp + progDetails.Size));
        }
        else
        {
          DebugTf("ERROR: Cannot read [%s]!\r\n");
          sprintf(progDetails.Strt, "%s", "????");
          sprintf(progDetails.End,  "%s", "????");
        }

        DebugTf("> [%d] [%s] [%s] [%s]\r\n", fileID, progDetails.Name, progDetails.Strt, progDetails.End);
        snprintf(filePath, sizeof(filePath), "/%s/%s.lck", progDetails.ID, cBuff);
        lckFile = LittleFS.open(filePath, "r");
        if (lckFile) 
        { 
            progDetails.Lock = true;
            lckFile.close();
        }
        else  progDetails.Lock = false;
        return true;
      }
    }
    
    return false;
    
}   // readProgDetailsByID()


//------------------------------------------------------
uint8_t findFirstEmptyID() 
{
  int slotNr;

  SPrintln("\r\nFind First Empty ID on Tape\r");
  snprintf(scrollName, sizeof(scrollName), "%s", "        <<        <<");
  scrollProgressTape(true);

  for(slotNr=0; slotNr<0xFF;slotNr++)
  {
    yield();
    scrollProgressTape(true);
    Debugf("test catalog[%d] => [%s]\r\n", slotNr, catalog[slotNr] ? "true":"false");
    if (!catalog[slotNr]) break;
    
  } // for ..

  if (slotNr >= 0xFF)
  {
    SPrintln("There are no more Empty Slot");
    return 0;
  }
  
  snprintf(progDetails.Name, sizeof(progDetails.Name), "<EmptySlot>");
  snprintf(progDetails.ID, sizeof(progDetails.ID), "%02x", slotNr);
  progDetails.numID = slotNr;
  progDetails.Lock  = false;
  progDetails.Size  = 0;
  sprintf(progDetails.Strt, "%s", "????");
  sprintf(progDetails.End,  "%s", "????");

  actFileID = slotNr;
  
  return slotNr;
    
}   // findFirstEmptyID()


//------------------------------------------------------
uint8_t findNextID(uint8_t actID) 
{
  int slotNr;
  
  snprintf(scrollName, sizeof(scrollName), "%s", ">>        >>        ");
  scrollName[sizeof(scrollName)] = 0;
  scrollProgressTape(true);

  if (!actCatalog)  readCatalog();
  
  savFileID = actID;
  if (actID < 0xFF) 
        actID++;
  else  actID = 0xFF;

  for(slotNr=actID; slotNr<0xFF;slotNr++)
  {
    yield();
    scrollProgressTape(true);
    Debugf("test catalog[%d] => [%s]\r\n", slotNr, catalog[slotNr] ? "true":"false");
    if (catalog[slotNr]) break;
    
  } // for ..
  
  if (slotNr >= 0xFF)
        return savFileID;
  else  return slotNr;
    
} //  findNextID()


//------------------------------------------------------
uint8_t findPreviousID(uint8_t actID) 
{
  int slotNr;
  
  snprintf(scrollName, sizeof(scrollName), "%s", "        <<        <<");
  scrollProgressTape(false);

  SPrintln("Find Previous ID on Tape\r");

  if (!actCatalog)  readCatalog();
  
  savFileID = actID;
  if (actID > 0x00) 
        actID--;
  else  actID = 0x00;

  for(slotNr=actID; slotNr>=0; slotNr--)
  {
    yield();
    scrollProgressTape(true);
    Debugf("test catalog[%d] => [%s]\r\n", slotNr, catalog[slotNr] ? "true":"false");
    if (catalog[slotNr]) break;
    
  } // for ..
  
  if (slotNr < 0x00)
        return savFileID;
  else  return slotNr;

} //  findPreviousID()


//------------------------------------------------------
void renameProgName(uint8_t fileNum, char *newName) 
{
    char cIn;
    char iDir[6];
    char oldNameHex[50];
    char oldNameLck[50];
    char oldNameTxt[50];

    if (fileNum == 0 || fileNum >= 0xE0) 
    {
        SPrintln("\r\nError: cannot alter name of this program\r\n");
        return;
    }
    readProgDetailsByID(fileNum);

    for(int i=0; i<strlen(newName); i++)
    {
      if (newName[i] == ' ')   newName[i] = '_';
    }

    sprintf(oldNameHex, "/%s/%s.hex", progDetails.ID, progDetails.Name);
    sprintf(oldNameLck, "/%s/%s.lck", progDetails.ID, progDetails.Name);
    sprintf(oldNameTxt, "/%s/%s.txt", progDetails.ID, progDetails.Name);
    sprintf(idDir, "/%s", progDetails.ID);

    if (progDetails.Lock) 
    {
        SPrintln("\nError: program is locked!\n");
        return;
    }
    DebugTf("oldNameHex[%s] ", oldNameHex);

    snprintf(filePath, sizeof(filePath), "/%s/%s.hex", progDetails.ID, newName);
    SPrintf("rename(%s, %s)\r\n", oldNameHex, filePath);
    if (LittleFS.rename(oldNameHex, filePath))
          SPrintln("OK");
    else  SPrintln("ERROR!");
    snprintf(filePath, sizeof(filePath), "/%s/%s.txt", progDetails.ID, newName);
    DebugTf("rename(%s, %s)\r\n", oldNameTxt, filePath);
    LittleFS.rename(oldNameTxt, filePath);
    snprintf(progDetails.Name, sizeof(progDetails.Name), newName);
    
}   // renameProgName()


//------------------------------------------------------
void eraseProgByID(uint8_t fileNum) 
{
    char iDir[6];
    char delNameHex[50];
    char delNameLck[50];
    char delNameTxt[50];

    readProgDetailsByID(fileNum);

    sprintf(delNameHex, "/%s/%s.hex", progDetails.ID, progDetails.Name);
    sprintf(delNameLck, "/%s/%s.lck", progDetails.ID, progDetails.Name);
    sprintf(delNameTxt, "/%s/%s.txt", progDetails.ID, progDetails.Name);
    sprintf(idDir, "/%s", progDetails.ID);

    DebugTf("delNameHex[%s] \r\n", delNameHex);
    SPrintf("delete [%s]\r\n", progDetails.Name);
    
    LittleFS.remove(delNameHex);
    LittleFS.remove(delNameLck);
    LittleFS.remove(delNameTxt);
    LittleFS.remove(iDir);

}   // eraseProgByID()


//------------------------------------------------------
// list files in /ID map
//------------------------------------------------------
void listProgramFiles() 
{
    uint8_t fileCount = 0;
    uint8_t lenName;
    String  space = "                           ";
    uint8_t savFileID = actFileID;
    int     slotNr, IDnum;
                            
    SPrintln("\r\nList files on Tape\r");
    webSocket.broadcastTXT("blockButtons");
    displayMsg("Wait .. \nListing Files\nTakes some time"); 

    Dir dir;
    for(slotNr=0; slotNr<=254; slotNr++)
    {
      if (!catalog[slotNr]) continue;
      IDnum = slotNr;
      sprintf(idDir, "/%02x", IDnum);
      arrayToUpper(idDir, strlen(idDir));
      //DebugTf("check id[%s] ..\r\n", idDir);      
      dir = LittleFS.openDir(idDir);

      while (dir.next()) 
      {
        yield();
        if (dir.fileName().endsWith(".lck")) continue;
        if (dir.fileName().endsWith(".txt")) continue;
        fileCount++;
        //lenName = (25 - dir.fileName().length());
        fileName2Name(IDnum, dir.fileName().c_str());
        //DebugTf("> [%s]\r\n", progDetails.Name);
        //File f = dir.openFile("r");
        //SPrint(space.substring(0, (25 - dir.fileName().length())));
        SPrintf("[%s] %s\r\n", progDetails.ID, progDetails.Name);
            
      } // while ..
    }
    SPrintf("\r\nThere are %d files on tape\n", fileCount);
    actFileID = savFileID;

    webSocket.broadcastTXT("freeButtons");

}   // listProgramFiles()


//------------------------------------------------------
// display description (.txt) file in /ID map
//------------------------------------------------------
void showDescription() 
{
    SPrintf("\r\nshow Description for %s\r\n", progDetails.Name);
    snprintf(cBuff, sizeof(cBuff), "/%s/%s.txt", progDetails.ID, progDetails.Name);
    DebugTf("try to open [%s] ..", cBuff);
    File D = LittleFS.open(cBuff, "r");
    if (D)
    {
      Debug(" YES!");
      while (D.available())
      {
        memset(cBuff, 0, sizeof(cBuff));
        D.readBytesUntil('\n', cBuff, sizeof(cBuff));
        SPrintln(cBuff);
      }
      D.close();
    }
    else
    {
      Debug(" .. not found");
    }
}   // showDescription()


//------------------------------------------------------
void toggleLock(uint8_t fileNum) {
    char cIn;
    
    readProgDetailsByID(fileNum);

    if (fileNum == 0 || fileNum >= 0xE0) return;

    snprintf(filePath, sizeof(filePath), "/%s/%s.lck", progDetails.ID, progDetails.Name);
    if (progDetails.Lock) 
    {
        LittleFS.remove(filePath);
    } else {
        File fileLock = LittleFS.open(filePath, "a");
        fileLock.close();
    }

    readProgDetailsByID(fileNum);

}   // toggleLock()


//------------------------------------------------------
bool getNewProgName() 
{
    snprintf(newName, sizeof(newName), progDetails.Name);
    
    if (actFileID == 0 || actFileID >= 0xE0) 
    {
        SPrintln("\r\nError: cannot alter Name of Program 00 (ScratchBook)\r\n");
        return false;
    }

    SPrintf("\nEnter new Name for Program [%s] ? ", newName);
    while (Serial.available()) 
    {
      cIn = Serial.read();    // empty buffer
    }
    while (TelnetStream.available()) 
    {
      cIn = TelnetStream.read();    // empty buffer
    }
    tmpB = 0;
    cIn  = 0;
    memset(newName, 0, sizeof(newName));
    while (cIn != '\r' && cIn != '\n' && tmpB < 19) 
    {
      yield();
      if (Serial.available()) 
      {
        cIn = Serial.read();
        if (cIn == ' ') cIn = '_';
        if (   (cIn >= '0' && cIn <= '9') 
            || (cIn >= 'A' && cIn <= 'Z')
            || (cIn >= 'a' && cIn <= 'z')
            || (cIn == '-') || (cIn == '_')
            ) 
        {
            newName[tmpB++] = cIn;
        }
      }
      if (TelnetStream.available()) 
      {
        cIn = TelnetStream.read();
        if (cIn == ' ') cIn = '_';
        if (   (cIn >= '0' && cIn <= '9') 
            || (cIn >= 'A' && cIn <= 'Z')
            || (cIn >= 'a' && cIn <= 'z')
            || (cIn == '-') || (cIn == '_')
            ) 
        {
            newName[tmpB++] = cIn;
        }
      }
    }
    SPrintln();
    newName[tmpB] = '\0';
    if (strlen(newName) == 0)
    {
      SPrintf("Do you really want to delete [%s] (Y/N)?", progDetails.Name);
      cIn = '0';
      while (cIn != 'Y' && cIn != 'N')
      {
        yield();
        if (Serial.available())
        {
          cIn = Serial.read();
        }
        if (TelnetStream.available())
        {
          cIn = TelnetStream.read();
        }
        cIn = toupper(cIn);
      }
      SPrintln();
      
      if (cIn == 'Y') 
      {
        SPrintf("\r\nOk, deleting [%s]\r\n", progDetails.Name);
        eraseProgByID(progDetails.numID);
        actFileID = findNextID(actFileID);
        scrollInterval = millis();
        return false;
      }
      else
      {
        return false;
      }
    }

    SPrintf("New Name is [%s]\r\n", newName);
    return true;
    
} // getNewProgName()

//------------------------------------------------------
void printMenu() {
    int8_t sp, p;
    
    SPrint("\nMenu ");
    for (p=strlen(progDetails.Name); p<22; p++) { SPrint(" "); }
    SPrintf("[%s]", progDetails.ID); 
    if (progDetails.Lock)   SPrint("* ");
    else                    SPrint("  ");
    SPrintf("- %s", progDetails.Name);
    SPrintf(" [$%s][$%s]\r\n", progDetails.Strt, progDetails.End);
    SPrintln("--------------------------------------------------\r");
    SPrintln("   P    - Play from Tape (uKIM enter [$1873 G])\r");
    SPrintln("   R    - Record to Tape (uKIM enter [$1800 G])\r");
    SPrintln("   +    - FastForw. to next file on tape\r");
    SPrintln("   -    - FastRev. to previous file on tape\r");
    SPrintf( "   C    - Change Name for Program [%s]\r\n", progDetails.ID);
    SPrintf( "   D    - Show Description of Program [%s]\r\n", progDetails.Name);
    SPrintln("   F    - Find first Free Slot\r");
    SPrint(  "   V    - toggle Verbose (is now ");
    if (doVerbose) SPrintln("ON)\r");
    else           SPrintln("OFF)\r");
    SPrintf( "   T    - Toggle Lock for Program [%s]\r\n", progDetails.ID);
    SPrintln("   L    - List files on tape\r");
    SPrintln("   H    - This menu\r");
    SPrintln("\r"); 
    SPrint(" (P/R/L/D/T/H) ? ");

  displayProgDetails();
  sendProgDetails();
  sendProgDescription();              
    
}   // printMenu()


//------------------------------------------------------
void setup() 
{
  Serial.begin(19200);
  while(!Serial) { delay(10); }
  Serial.println();
  Serial.println("\r\n[uTapeEmulator (v2.0)]");
  Serial.flush();
  pinMode(_LED_PIN,       OUTPUT);
  digitalWrite(_LED_PIN,  HIGH);
  pinMode(_PLL_RESET,     OUTPUT);
  pinMode(_TAPE_OUT_PIN,  OUTPUT);
  pinMode(_PLAY_BUTTON,   INPUT);
  pinMode(_FFWD_BUTTON,   INPUT);
  pinMode(_FREV_BUTTON,   INPUT);

  turnOnOledDisplay();
  delay(1000);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  displayMsg("SetupWiFi..");

  startWiFi(_HOSTNAME, 240);

  String Msg = "Connected to:\r\n"+WiFi.SSID()+"\r\n"+WiFi.localIP().toString();
  Debugln(Msg);
  displayMsgAndWait(Msg , 5);
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.setFont(ArialMT_Plain_16);      // _24
  //display.drawString(0, 12, WiFi.SSID());
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.display();
  //display.drawString(0, 32, WiFi.localIP().toString());
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.display();
  //delay(2000);

  //display.init();
  //display.flipScreenVertically();
  //display.display();

  startTelnet();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  MDNS.port(81);  // webSockets

  SPrintln("\r\nLittleFS.begin() ..");
  setupFS();

  moveHexFilesFromRoot();

  /****
  httpServer.on("/",               indexPage);
  httpServer.on("/index",          indexPage);
  httpServer.on("/index.html",     indexPage);
  ****/
  httpServer.serveStatic("/",               LittleFS, "/index.html");
  httpServer.serveStatic("/index",          LittleFS, "/index.html");
  httpServer.serveStatic("/index.html",     LittleFS, "/index.html");
  httpServer.serveStatic("/index.css",      LittleFS, "/index.css");
  httpServer.serveStatic("/index.js",       LittleFS, "/index.js");
  httpServer.serveStatic("/FSexplorer",     LittleFS, "/littleFSexplorer.html");
  httpServer.serveStatic("/FSexplorer.png", LittleFS, "/FSexplorer.png");

  httpServer.begin();
  SPrintln("HTTP httpServer started");

  startMDNS(_HOSTNAME);

  processKeyTimer    = millis();
  processButtonTimer = millis();
  dimOledTimer       = millis();

  readCatalog();

  doVerbose = false;
    
  SPrintln();
  readProgDetailsByID(actFileID);

  printMenu();

  //--- reset ATtinyPLL ---------
  digitalWrite(_PLL_RESET, LOW);
  delay(10);
  digitalWrite(_PLL_RESET, HIGH);

}   // setup()


//------------------------------------------------------
void loop() 
{
  httpServer.handleClient();
  MDNS.update();
  webSocket.loop();

  //--- read key's from monitor and telnet
  if (Serial.available()) 
  {
    cIn = Serial.read();
    if ( (cIn > ' ' && cIn <= 'z') || (cIn == '\n') ) 
    {
      mChoice = toupper(cIn);
    }
  }
  if (TelnetStream.available()) 
  {
    cIn = TelnetStream.read();
    if ( (cIn > ' ' && cIn <= 'z') || (cIn == '\n') ) 
    {
      mChoice = toupper(cIn);
    }
  }

  if ((millis() - processButtonTimer) > _BUTTONTIME)
  {
    processButtonTimer = millis();
    if (buttonChanged())
    {
      DebugTf("Button Pressed value is [%c]\r\n", buttonChoice);  
    }
  }

  if ((millis() - dimOledTimer) > _DIMOLEDTIME) 
  {
    turnOffOledDisplay();
    
  }
  else if (millis() > scrollInterval) 
  {
    if (idxName == 0) scrollInterval = millis() + _HIGHSCROLLTIME;
    else              scrollInterval = millis() + _LOWSCROLLTIME;
    scrollProgName();
  }
  
  if (buttonChoice != 0) 
  {
        mChoice = buttonChoice;
        buttonChoice = 0;
  }
  if (   mChoice == 'C' || mChoice == 'D' || mChoice == 'L' 
      || mChoice == 'P' || mChoice == 'R' || mChoice == 'T'
      || mChoice == 'V' || mChoice == 'H' || mChoice == 'F' 
      || mChoice == '+' || mChoice == '-'
     ) 
  {
    if (displayIsOff) 
    {
      turnOnOledDisplay();
      printMenu();
      mChoice = 0;
 
    } else 
    {
      SPrintf(" choice is [%c]\r\n", mChoice);
      switch (mChoice)
      {
        case 'C':   if (getNewProgName())
                    {
                      renameProgName(actFileID, newName);
                    }
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'D':   showDescription();
                    printMenu();
                    break;
        case 'F':   actFileID = findFirstEmptyID();
                    //-readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'L':   listProgramFiles();
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'P':   playbackTape();
                    printMenu();
                    break;
        case 'R':   recordTape();
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'T':   toggleLock(actFileID);
                    printMenu();
                    break;
        case 'V':   doVerbose = !doVerbose;
                    printMenu();
                    break;
        case '+':   actFileID = findNextID(actFileID);
                    readProgDetailsByID(actFileID);
                    scrollInterval = millis();
                    printMenu();
                    break;
        case '-':   actFileID = findPreviousID(actFileID);
                    scrollInterval = millis();
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        default:    printMenu();
      } // switch..
      
      if (Serial.available())       Serial.read();
      if (TelnetStream.available()) TelnetStream.read();

      mChoice = 0;
      dimOledTimer = millis();
    } // else ..
  } // if ..
    
}   // loop()

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
***************************************************************************/
