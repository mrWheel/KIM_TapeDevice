/*
***************************************************************************  
 *  Program  : digitalTapeRecorder (for the KIM-1 and microKIM)
 *  Copyright (c) 2021 Willem Aandewiel
 */
#define _FW_VERSION "v2.0.0 WS (24-02-2021)"
/* 
*  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************  
 * Board:         "Generic ESP8266 Module"  // always! even if you have a Wemos or NodeMCU!
 * Builtin Led:   "2"
 * CPU Frequency: "80 MHz" or "160 MHz"
 * Flash Size:    "4MB (FS:3MB OTA:~512KB)"
***************************************************************************/  

#define USE_UPDATE_SERVER
#define DEBUG_ON
#define _HAS_BUTTONS    true    // false=no buttons, true=yes we have buttons
#define _REAL_KIM1      false   // is it a "real" KIM-1 (invert bits out)?
#define _LOCK_FROM      0xDD    // from this slot on every slot is locked

//---------- no need to change enything after this ------

#define _HOSTNAME   "kimDTR"

#include <TelnetStream.h>
#include "Debug.h"
#include "networkStuff.h"
#include "LittleFS.h"

#include <SSD1306.h>    // https://github.com/ThingPulse/esp8266-oled-ssd1306

//#define _BV(bit)            (1 << (bit))
#define _SET(a,b)           ((a) |= _BV(b))
#define _CLEAR(a,b)         ((a) &= ~_BV(b))

#define _LED_PIN        LED_BUILTIN // dont use this pin!

#define _SDA            4       // D2   - GPIO04
#define _SCL            5       // D1   - GPIO05

#define _PLL_RESET      2       // D4   - GPIO02  // OEPS!! Also the Builtin LED
#define _PLL_IN_PIN     13      // D7   - GPIO13  
#define _TAPE_OUT_PIN   12      // D6   - GPIO12  

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
char            cBuff[90], sBuff[30], scrollName[50];
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
//-- pull _PLL_RESET to GND for 50ms
//------------------------------------------------------
void resetATtinyPLL() 
{
  delay(50);
  digitalWrite(_PLL_RESET, LOW);
  delay(50);
  digitalWrite(_PLL_RESET, HIGH);

}   // resetATtinyPLL()


//------------------------------------------------------
//-- checks all buttons to see if any-one is pressed
//-- return:
//--    true  if a change occurred
//--    false if no button is pressed
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
        delay(50);
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
            delay(50);
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
        delay(50);
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
        delay(50);
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
//-- input: 
//--    complete filename ("filename.hex")
//--    decimal value of Slot ("84")
//-- sets 
//--    progDetails.ID to "54" (= hex value of 84)
//--    progDetails.Name to "filename" (without the extension)
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
//-- reads a "nibble" (which is a char {0-9, A-F})
//-- returns
//--    the nibble as a (ascii) char 
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
//-- reads all Slot's ("/00" to "/FF") that are 
//-- in the form "XX" where "X" is {0-9,A-F} and if
//-- such a Slot exists set's calalog[Slot] to true
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
//-- reads a program file and deturments the actual 
//-- filesize 
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
//-- at startup and if the FSexplorer is used looks
//-- for ".hex" files in the root directory.
//-- If one is found it will activate the first free
//-- Slot (map) and copies the .hex file from the root
//-- directory to the empty Slot.
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
      //-- baseName.txt ---
      snprintf(cBuff, (NAME_SIZE), "%s.txt", baseName);
      DebugTf("(2) cBuff[%s]\r\n", cBuff);
      sprintf(filePath, "%s/%s", idDir, cBuff);
      DebugTf("rename(%s -> %s)\r\n", cBuff, filePath);
      LittleFS.rename(cBuff, filePath);
      //-- baseName.ptp ---
      snprintf(cBuff, (NAME_SIZE), "%s.ptp", baseName);
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
//-- creates or removes a ".lck" file in the Slot (map)
//-- with the (decimal) value "numID"
//------------------------------------------------------
void writeLockFile(uint16_t numID, bool newState)
{
  DebugTf("change state [%s.lck] to [%s]\r\n", progDetails.Name, newState? "Lock":"R/W");
  if (numID == 0 || numID >= _LOCK_FROM) 
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
//-- create or overwrite a file with the given extension
//------------------------------------------------------
void writeExternalFile(uint8_t numID, const char ext[], String data) 
{
  DebugTf("write to [%s.txt]\r\n", progDetails.Name);
  
  if (numID == 0 || numID >= _LOCK_FROM) 
  {
      SPrintf("\r\nError: cannot alter %s of this program\r\n", ext);
      return;
  }
  readProgDetailsByID(numID);

  snprintf(filePath, sizeof(filePath), "/%s/%s.%s", progDetails.ID, progDetails.Name, ext);
  DebugTf("open(%s, \"w\")\r\n", filePath);
  File fileData = LittleFS.open(filePath, "w");
  for(int p=0; p<data.length(); p++)
  {
    fileData.write(data[p]);
  }
  fileData.close();

} //  writeExternalFile()


//------------------------------------------------------
//-- input:
//--    fileID (decimal value)
//-- sets:
//--    progDetails struct
//-- returns:
//--    true if al went well
//--    false otherwise
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
      //if (dir.fileName().endsWith(".lck")) continue;
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
          arrayToUpper(progDetails.Strt, 4);
          DebugTf("=>[Strt]>[$%s]\r\n", progDetails.Strt);
          fileData.close();
          DebugTf("=>[Size]>[$%04x]\r\n", progDetails.Size);
          int tmp = strtol(progDetails.Strt, NULL, 16);
          DebugTf("=>>[End]>[$%04x]\r\n", (tmp + progDetails.Size));
          snprintf(progDetails.End, sizeof(progDetails.End), "%04x", (tmp + progDetails.Size));
          arrayToUpper(progDetails.End, 4);
        }
        else
        {
          snprintf(wsSend, sizeof(wsSend), "ERROR: Cannot read [%s]!\r", filePath);
          DebugTln(wsSend);
          sendStatusMessage(wsSend);
          sprintf(progDetails.Strt, "%s", "????");
          sprintf(progDetails.End,  "%s", "????");
        }

        DebugTf("> [%d] [%s] [%s] [%s]\r\n", fileID, progDetails.Name, progDetails.Strt, progDetails.End);

        if (progDetails.numID >= _LOCK_FROM) 
        {
          progDetails.Lock = true;
        }
        else
        {
          snprintf(filePath, sizeof(filePath), "/%s/%s.lck", progDetails.ID, cBuff);
          lckFile = LittleFS.open(filePath, "r");
          if (lckFile || progDetails.numID >= _LOCK_FROM) 
          { 
            progDetails.Lock = true;
            lckFile.close();
          }
          else 
          {
            progDetails.Lock = false;
          }
        }
        return true;
      }
    }
    
    return false;
    
}   // readProgDetailsByID()


//------------------------------------------------------
//-- reads all Slots from "00" to "FF" until it finds a 
//-- Slot that does not exist
//-- returns:
//--    Slot number (decimal)
//------------------------------------------------------
uint8_t findFirstEmptyID() 
{
  int slotNr;

  SPrintln("\r\nFind First Empty ID on Tape\r");
  snprintf(scrollName, sizeof(scrollName), "%s", "        <<        <<");
  scrollProgressTape(true);

  if (!actCatalog)  readCatalog();

  for(slotNr=0; slotNr<0xFF;slotNr++)
  {
    yield();
    scrollProgressTape(true);
    DebugTf("test catalog[%d] => [%s]\r\n", slotNr, catalog[slotNr] ? "true":"false");
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
//-- find the next Slot ID starting from the current 
//-- Slot (actID) up to "FF"
//-- returns:
//--    slotNr (If a Slot is found)
//--    otherwise the actID
//------------------------------------------------------
uint8_t findNextID(uint8_t actID) 
{
  int slotNr, firstEmptyID = 0;
  
  snprintf(scrollName, sizeof(scrollName), "%s", ">>        >>        ");
  scrollName[sizeof(scrollName)] = 0;
  scrollProgressTape(true);

  if (!actCatalog)  readCatalog();
  
  savFileID = actID;

  //-- find FirstEmptyID ----
  for(firstEmptyID=0; firstEmptyID<0xFF;firstEmptyID++)
  {
    yield();
    scrollProgressTape(true);
    DebugTf("find firstEmptyID in catalog[%d] => [%s]\r\n"
                                            , firstEmptyID
                                            , catalog[firstEmptyID] ? "true":"false");
    //-- found the First Empty ID???
    if (!catalog[firstEmptyID]) break;  // YES!

  } // for ..

  if (actID < 0xFF) 
        actID++;
  else  actID = 0xFF;

  for(slotNr=actID; slotNr<=0xFF;slotNr++)
  {
    yield();
    scrollProgressTape(true);
    //DebugTf("find nextNr in catalog[%d] => [%s]\r\n", slotNr, catalog[slotNr] ? "true":"false");
    if (catalog[slotNr]) break; // found the nextID
    
  } // for ..

  if ( (firstEmptyID > savFileID) && (firstEmptyID < slotNr) )
  {
    DebugTf("First emptyID between [%d] and [%d]\r\n", savFileID, slotNr);
    slotNr = findFirstEmptyID();   // a bit clumsy. Sorry
  }
  if (slotNr > 0xFF)
        return savFileID;
  else  return slotNr;
    
} //  findNextID()


//------------------------------------------------------
//-- find the previous Slot ID starting from the current 
//-- Slot (actID) down to "00"
//-- returns:
//--    slotNr (If a Slot is found)
//--    otherwise the actID
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
//-- rename a file with the ext extension in Slot "ID"
//-- after this function the file
//--    "oldName.ext" is renamed to "newName.ext"
//------------------------------------------------------
void renameOneFile(char *ID, char *oldName, char *newName, const char ext[]) 
{
  char oldNamePath[50];
  char newNamePath[50];
    
  snprintf(oldNamePath, sizeof(oldNamePath), "/%s/%s.%s", ID, oldName, ext);
  snprintf(newNamePath, sizeof(newNamePath), "/%s/%s.%s", ID, newName, ext);

  if (LittleFS.exists(oldNamePath))
  {
    SPrintf("rename [%s] to [%s] ..", oldNamePath, newNamePath);
    if (LittleFS.rename(oldNamePath, newNamePath))
          SPrintln("OK");
    else  SPrintln("ERROR!");
  }
  
} //  renameOneFile()

//------------------------------------------------------
//-- renames all the files in a Slot (fileNum - decimal)
//-- from the progDetails.Name to the "newName"
//-- if "newName" is "DEL" all the files in the Slot
//-- will be removed and than the Slot (map) is removed 
//------------------------------------------------------
void renameFilesByID(uint8_t fileNum, char *newName) 
{
  if (fileNum == 0 || fileNum >= _LOCK_FROM) 
  {
    SPrintln("\r\nError: cannot alter name of this program\r\n");
    return;
  }
  readProgDetailsByID(fileNum);

  if (progDetails.Lock) 
  {
    SPrintln("\nError: program is locked!\n");
    return;
  }

  for(int i=0; i<strlen(newName); i++)
  {
    if (newName[i] == ' ')   newName[i] = '_';
  }

  DebugTf("newName [%s]\r\n", newName);
  if (strncmp(newName, "DEL", 3) == 0)
  {
    snprintf(wsSend, sizeof(wsSend), "delete [%s]/[%s]\r", progDetails.ID, newName);
    DebugTln(wsSend);
    sendStatusMessage(wsSend);
    removeID(progDetails.numID);
    actCatalog = false;
    return;
  }
  
  DebugTf("rename [%s]/[%s] to [%s]\r\n", progDetails.ID, progDetails.Name, newName);
  renameOneFile(progDetails.ID, progDetails.Name, newName, "hex");
  renameOneFile(progDetails.ID, progDetails.Name, newName, "txt");
  renameOneFile(progDetails.ID, progDetails.Name, newName, "ptp");
  renameOneFile(progDetails.ID, progDetails.Name, newName, "asm");
      
  actCatalog = false;

}   // renameFilesByID()


//------------------------------------------------------
//-- removes a file "oldName" from Slot (ID hex)
//------------------------------------------------------
void removeOneFile(char *ID, const char *oldName) 
{
  char oldNamePath[50];
  
  snprintf(oldNamePath, sizeof(oldNamePath), "/%s/%s", ID, oldName);

  if (LittleFS.exists(oldNamePath))
  {
    SPrintf("remove [%s] ..", oldNamePath);
    if (LittleFS.remove(oldNamePath))
          SPrintln("OK");
    else  SPrintln("ERROR!");
  }
  
} //  removeOneFile()

//------------------------------------------------------
//-- removes all files in Slot (fileNum - decimal)
//-- and when the Slot is empty the Slot (map) itself
//-- will be removed
//------------------------------------------------------
void removeID(uint8_t fileNum) 
{
  char iDir[5];

  DebugTf("delete [%d]\r\n", fileNum);

  if (fileNum == 0 || fileNum >= _LOCK_FROM) 
  {
    SPrintln("\r\nError: cannot remove this program\r\n");
    return;
  }
    
  readProgDetailsByID(fileNum);

  if (progDetails.Lock) 
  {
    SPrintln("\nError: program is locked!\n");
    return;
  }

  DebugTf("remove [%s]/[%s]\r\n", progDetails.ID, progDetails.Name);

  sprintf(idDir, "/%s", progDetails.ID);
  Dir dir = LittleFS.openDir(idDir);
  while (dir.next()) 
  {
    removeOneFile(progDetails.ID, dir.fileName().c_str());
  }
  LittleFS.remove(iDir);
  
  actCatalog = false;
  
  actFileID = findPreviousID(progDetails.numID);

}   // removeID()


//------------------------------------------------------
// list files in /ID map
//------------------------------------------------------
void listProgramFiles(const char *to) 
{
    uint8_t fileCount = 0;
    uint8_t lenName;
    String  space = "                           ";
    uint8_t savFileID = actFileID;
    int     slotNr, IDnum;
    char    komma = ' ';
                            
    SPrintf("\r\nList files on Tape to [%s]\r\n", to);
    webSocket.broadcastTXT("blockButtons");
    displayMsg("Wait .. \nListing Files\nTakes some time"); 
 
    if (!actCatalog)  readCatalog();
    
    String jsonString  = "{";
    jsonString += "\"msgType\":\"catalog\"";
    jsonString += ", \"programs\":[";

    Dir dir;
    for(slotNr=0; slotNr<=255; slotNr++)
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
        if (dir.fileName().endsWith(".hex"))
        {
          fileCount++;
          fileName2Name(IDnum, dir.fileName().c_str());
          SPrintf("[%s] - [%s]\r\n", progDetails.ID, progDetails.Name);
          snprintf(wsSend, sizeof(wsSend), "%c{\"ID\":\"%s\", \"name\":\"%s\"}%c\r\n", komma, progDetails.ID, progDetails.Name);
          jsonString += String(wsSend);
          komma = ',';
        }      
      } // while ..
    }
    SPrintf("\r\nThere are %d files on Tape\n", fileCount);

    webSocket.broadcastTXT("freeButtons");
    jsonString += "]";
    jsonString += "}";

    if (strncmp(to, "ws", 2) == 0)
    {
      Debugln(jsonString);
      webSocket.broadcastTXT(jsonString.c_str());
    }

    actFileID = savFileID;

}   // listProgramFiles()


//------------------------------------------------------
//-- prints description (.txt) file in /ID map
//-- to telnet and Serial
//------------------------------------------------------
void showDescription() 
{
    SPrintf("\r\nshow Description for %s\r\n", progDetails.Name);
    SPrintln("--------------------------------------------------------------------------\r");
    snprintf(cBuff, sizeof(cBuff), "/%s/%s.txt", progDetails.ID, progDetails.Name);
    DebugTf("try to open [%s] ..", cBuff);
    File D = LittleFS.open(cBuff, "r");
    if (D)
    {
      Debug(" YES!");
      while (D.available())
      {
        memset(cBuff, 0, sizeof(cBuff));
        int c = D.readBytesUntil('\n', cBuff, 62);
        if (c >= 60) cBuff[62] = 0;
        SPrintln(cBuff);
      }
      D.close();
      SPrintln("--------------------------------------------------------------------------\r");
    }
    else
    {
      Debug(" .. not found");
    }
}   // showDescription()


//------------------------------------------------------
//-- creates or removes ".lck" file
//------------------------------------------------------
void toggleLock(uint8_t fileNum) {
    char cIn;
    
    readProgDetailsByID(fileNum);

    if (fileNum == 0 || fileNum >= _LOCK_FROM) return;

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
//-- prompts the user at telnet and Serialto enter a new 
//-- program name
//------------------------------------------------------
bool getNewProgName() 
{
  uint32_t maxWaitForKeyTimer;
  
  snprintf(newName, sizeof(newName), progDetails.Name);
    
  if (actFileID == 0) 
  {
    SPrintln("\r\nError: cannot alter Name of Program 00 (ScratchBook)\r\n");
    return false;
  }
    
  if (actFileID >= _LOCK_FROM) 
  {
    SPrintf("\r\nError: cannot alter Name of Program %02x \r\n", progDetails.ID);
    return false;
  }

  if (progDetails.Lock) 
  {
    SPrintln("\nError: program is locked!\n");
    return false;
  }

  SPrintf("\nEnter new Name for Program [%s] ? ", newName);

  //--- empty Serial buffer ...
  while (Serial.available()) 
  {
    cIn = Serial.read(); 
  }
  //--- empty TelnetStream buffer ...
  while (TelnetStream.available()) 
  {
    cIn = TelnetStream.read();
  }
  tmpB = 0;
  cIn  = 0;
  memset(newName, 0, sizeof(newName));
  maxWaitForKeyTimer = millis();
  while (   (cIn != '\r' && cIn != '\n' && tmpB < 19)
         && ((millis() - maxWaitForKeyTimer) < 10000)
        ) 
  {
    yield();
    if (Serial.available()) 
    {
      maxWaitForKeyTimer = millis();
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
      maxWaitForKeyTimer = millis();
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
  if (strlen(newName) < 3)
  {
    SPrintln("Not a valid name (must be 3 chars or longer)!");
    return false;    
  }
  if (strncmp(newName, "DEL", 3) == 0)
  {
    SPrintf("Do you really want to delete [%s] (Y/N)?", progDetails.Name);
    cIn = '0';
    maxWaitForKeyTimer = millis();
    while (   (cIn != 'Y' && cIn != 'N')
           && ((millis() - maxWaitForKeyTimer) < 10000)
          ) 
    {
      yield();
      if (Serial.available())
      {
        maxWaitForKeyTimer = millis();
        cIn = Serial.read();
      }
      if (TelnetStream.available())
      {
        maxWaitForKeyTimer = millis();
        cIn = TelnetStream.read();
      }
      cIn = toupper(cIn);
    }
    SPrintln();
      
    if (cIn == 'Y') 
    {
      SPrintf("\r\nOk, deleting [%s]\r\n", progDetails.Name);
      removeID(progDetails.numID);
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
//-- prints the menu to telnet and Serial
//------------------------------------------------------
void printMenu() 
{
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
  SPrintln("   +    - FastForw. to next file on Tape\r");
  SPrintln("   -    - FastRev. to previous file on Tape\r");
  SPrintf( "   C    - Change Name for Program [%s] (\"DEL\" to delete)\r\n", progDetails.ID);
  SPrintf( "   D    - Show Description of Program [%s]\r\n", progDetails.Name);
  SPrintln("   F    - Find first Free Slot\r");
  SPrint(  "   V    - toggle Verbose (is now ");
  if (doVerbose) SPrintln("ON)\r");
  else           SPrintln("OFF)\r");
  SPrintf( "   T    - Toggle Lock for Program [%s]\r\n", progDetails.ID);
  SPrintln("   L    - List files on Tape\r");
  SPrintln("   H    - This menu\r");
  SPrintln("\r"); 
  SPrint(" (P/R/L/D/T/H) ? ");

  displayProgDetails();
  sendProgDetails();
  webSocket.broadcastTXT("freeButtons");
    
}   // printMenu()


//------------------------------------------------------
void setup() 
{
  Serial.begin(19200);
  while(!Serial) { delay(10); }
  Serial.println();
  Serial.println("\r\n[digitalTapeRecorder (v2.0)]");
  Serial.flush();
  pinMode(_PLL_RESET,     OUTPUT);  // this is also the BuiltIn LED pin
  pinMode(_TAPE_OUT_PIN,  INPUT_PULLUP); // set to OUTPUT only when sending to KIM
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

  startTelnet();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  MDNS.port(81);  // webSockets

  SPrintln("\r\nLittleFS.begin() ..");
  setupFS();

  moveHexFilesFromRoot();

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

  resetATtinyPLL();

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
        case 'C':   webSocket.broadcastTXT("blockButtons");
                    if (getNewProgName())
                    {
                      renameFilesByID(actFileID, newName);
                    }
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'D':   showDescription();
                    printMenu();
                    break;
        case 'F':   webSocket.broadcastTXT("blockButtons");
                    actFileID = findFirstEmptyID();
                    printMenu();
                    break;
        case 'L':   webSocket.broadcastTXT("blockButtons");
                    listProgramFiles("console");
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'P':   webSocket.broadcastTXT("blockButtons");
                    playbackTape();
                    printMenu();
                    break;
        case 'R':   webSocket.broadcastTXT("blockButtons");
                    recordTape();
                    readProgDetailsByID(actFileID);
                    printMenu();
                    break;
        case 'T':   toggleLock(actFileID);
                    printMenu();
                    break;
        case 'V':   doVerbose = !doVerbose;
                    printMenu();
                    break;
        case '+':   webSocket.broadcastTXT("blockButtons");
                    actFileID = findNextID(actFileID);
                    readProgDetailsByID(actFileID);
                    scrollInterval = millis();
                    printMenu();
                    break;
        case '-':   webSocket.broadcastTXT("blockButtons");
                    actFileID = findPreviousID(actFileID);
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
****************************************************************************
*/
