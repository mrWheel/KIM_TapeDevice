/*
***************************************************************************  
**  Program  : digitalTapeRecorder v2.0 / Record
**  Copyright (c) 2021 - 2022 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/

#define _BIT_HALF_TIME  3750


//------------------------------------------------------
//-- reads char's from _ATtiny_PLL2 and writes them
//-- to the tempFile.
//-- After receiving all the bytes renames the tempFile
//-- to the "progDetails.Name".hex file
//------------------------------------------------------
void recordTape() 
{
    char tempFile[] = "/tmp/KIM.hex";
    
    SPrintln("\r\nSave program to Tape ..");
    //sendStatusMessage("Write program to Tape ..");

    displayMsg(0, "Recording..", 0);
    
    if (progDetails.Lock) 
    {
        SPrintln("Error: Program is locked (RO)!");
        sendStatusMessage("Error: Program is locked (RO)!");
        displayMsg(2, "** ReadOnly **", 2);
        return;
    }
    
    File dataFile = LittleFS.open(tempFile, "w");
      
    SPrintln("Swapping serial to read from ATtinyPLL\r\n");
    Serial.flush();
    
    // RxD will be re-routed from GPIO01 and GPIO03 to GPIO15 (TX) and GPIO13 (RX)
    Serial.swap();
    Serial.flush();

    //------------ wait for SYNC char's -------
    actByte = 0;
    //sendStatusMessage("Wait for SYNC ..");
    SPrint("Wait for SYNC ..");
    displayMsg(1, "Wait for SYNC ..", 0);

    getTimingDuration = millis();
    while ((actByte != 'S') && (millis() - getTimingDuration) < 10000)
    {
      if (Serial.available())
      {
        actByte = Serial.read();
        SPrint((char)actByte);
        getTimingDuration = millis();
      }
    }
    if ((millis() - getTimingDuration) >= 10000)
    {
      Serial.swap();  // swap() back to normal
      sendStatusMessage("Wait for SYNC ..Time Out!! Abort");
      SPrintln(".. time out; Abort");
      displayMsg(2, "time out; Abort.", 2);
      resetATtinyPLL();
      dataFile.close();
      return;
    }
    SPrintln(" .. received!");
    displayMsg(2, ".. received", 0);

    //------------ wait for Start (*)  char's -------
    //sendStatusMessage("Wait for '*' ..");
    SPrint("Wait for '*' ..");
    displayMsg(1, "Wait for '*' ..   ", 0);
    displayMsg(2, "                  ", 0);
    getTimingDuration = millis();
    while ((actByte != '*') && (millis() - getTimingDuration) < 6000)
    {
      if(Serial.available())
      {
        actByte = Serial.read();
        if ((char)actByte == '*')
        SPrint((char)actByte);
        getTimingDuration = millis();
      }
    }
    if ((millis() - getTimingDuration) >= 6000)
    {
      Serial.swap();  // swap() back to normal
      sendStatusMessage("Wait for '*' .. Time Out!! Abort");
      SPrintln(".. time out; Abort");
      displayMsg(2, "time out; Abort.", 2);
      dataFile.close();
      resetATtinyPLL();
      return;
    }
    SPrintln(" .. received!");
    displayMsg(2, ".. received!", 0);
    
    //------------ Now process data unit '/' -------
    //sendStatusMessage("Process program data ..");
    SPrintln("Process program data ..");
    displayMsg(2, "                  ", 0);
    displayMsg(1, "Processing data ..", 0);
    getTimingDuration = millis();
    while ((actByte != '/') && (millis() - getTimingDuration) < 500)
    {
      if(Serial.available())
      {
        actByte = Serial.read();
        if (actByte > 'F')
        {
          Serial.swap();  // swap() back to normal
          sendStatusMessage("Process program data .. ERROR!! $00F1 not '00'!! Abort");
          SPrintln(".. ERROR!! $00F1 not '00'; Abort");
          displayMsg(1, "Error!!! ..", 0);
          displayMsg(2, "$00F1 not '00' ", 2);
          dataFile.close();
          resetATtinyPLL();
          return;
        }
        dataFile.print((char)actByte);
        SPrint((char)actByte);
        getTimingDuration = millis();
      }
    }
    if ((millis() - getTimingDuration) >= 500)
    {
      Serial.swap();  // swap() back to normal
      sendStatusMessage("Process program data .. Time Out!! Abort");
      SPrintln(".. time out; Abort");
      displayMsg(2, ".. time out; Abort", 2);
      dataFile.close();
      resetATtinyPLL();
      return;
    }
    //sendStatusMessage("Process program data .. '/' received!");
    SPrintln(" .. '/' received!");
    displayMsg(1, "'/' received!", 0);
    
    //------------ Now process data unit '/' -------
    SPrint("checksum and EOT ..");
    getTimingDuration = millis();
    while ((millis() - getTimingDuration) < 500)
    {
      if(Serial.available())
      {
        actByte = Serial.read();
        dataFile.print((char)actByte);
        SPrint((char)actByte);
        getTimingDuration = millis();
      }
    }
    SPrintln(" .. done!!");
    displayMsg(2, ".. Done!", 1);
    
    // RxD will back to "normal"
    SPrintln("Swapping serial port to normal\r\n");
    Serial.swap();
    Serial.flush();
    dataFile.close();

    if (String(progDetails.Name) == "<EmptySlot>") 
    {
      snprintf(progDetails.Name, sizeof(progDetails.Name), "Program-%s", progDetails.ID);
    }
    snprintf(idDir, sizeof(idDir), "%s", progDetails.ID);
    arrayToUpper(idDir, strlen(idDir)); //-- Bug-05-08-2021
    sprintf(filePath, "/%s/%s.hex", progDetails.ID, progDetails.Name);

    SPrintf("\r\nremove [%s]\r\n", filePath);
    LittleFS.remove(filePath);
    LittleFS.mkdir(idDir);
    SPrintf("rename [%s] to [%s] .. ", tempFile, filePath);
    if (LittleFS.rename(tempFile, filePath))
          SPrintln("OK!");
    else  SPrintln("Error!");

    actCatalog = false;
    actFileID  = progDetails.numID;
    readProgDetailsByID(actFileID);
    
    SPrintln("\r\n\nDone..");

}   // recordTape()


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
