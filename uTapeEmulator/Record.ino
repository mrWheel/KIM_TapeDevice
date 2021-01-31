/*
***************************************************************************  
**  Program  : uTapeEmulator v2 / Record
**  Copyright (c) 2017 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/

#define _BIT_HALF_TIME  3750


//------------------------------------------------------
void recordTape() 
{
    char tempFile[] = "/tmp/KIM.hex";
    
    SPrintln("\r\nSave program to cassette tape ..");

    if (progDetails.Lock) 
    {
        SPrintln("Error: Program is locked (RO)!");
        displayMsgAndWait("** ReadOnly **", 2);
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
    SPrint("Wait for SYNC ..");
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
      SPrintln(".. time out; Abort");
      dataFile.close();
      return;
    }
    SPrintln(" .. received!");
    
    //------------ wait for Start (*)  char's -------
    SPrint("Wait for '*' ..");
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
      SPrintln(".. time out; Abort");
      dataFile.close();
      //--- reset ATtinyPLL ---------
      digitalWrite(_PLL_RESET, LOW);
      delay(10);
      digitalWrite(_PLL_RESET, HIGH);
      return;
    }
    SPrintln(" .. received!");
    
    //------------ Now process data unit '/' -------
    SPrintln("Process program data ..");
    getTimingDuration = millis();
    while ((actByte != '/') && (millis() - getTimingDuration) < 500)
    {
      if(Serial.available())
      {
        actByte = Serial.read();
        if (actByte > 'F')
        {
          Serial.swap();  // swap() back to normal
          SPrintln(".. ERROR!! $00F1 not '00'; Abort");
          dataFile.close();
          //--- reset ATtinyPLL ---------
          digitalWrite(_PLL_RESET, LOW);
          delay(10);
          digitalWrite(_PLL_RESET, HIGH);
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
      SPrintln(".. time out; Abort");
      dataFile.close();
      //--- reset ATtinyPLL ---------
      digitalWrite(_PLL_RESET, LOW);
      delay(10);
      digitalWrite(_PLL_RESET, HIGH);
      return;
    }
    SPrintln(" .. '/' received!");
    
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
