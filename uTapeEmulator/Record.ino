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
bool wait4Pin2Get(bool HiLo, uint32_t maxWait) {

    maxWait += millis();
    while(digitalRead(_TAPE_IN_PIN) == !HiLo) {
        yield(); 
        if (millis() > maxWait) {
            return false;
        }
    }

    return true;
    
}   // wait4Pin2Get();


//------------------------------------------------------
bool wait4Sync() {
    uint32_t    tillTime = millis() + 15000;

    SPrint("Wait for "); printByte(0x16); 
    while (millis() < tillTime) {
        yield();
        if (actByte == 0x16) {
            SPrintln(" found!");
            return true;
        } else {
            yield();
            actByte >>= (1);
            // wait for TAPE_IN to get High
            if (!wait4Pin2Get(HIGH, 1000)) {
                SPrint(".");
            }
            bitStartTime    = micros(); 
            // wait for TAPE_IN to get Low
            wait4Pin2Get(LOW, 100);
            bitTime         = micros() - bitStartTime;

            if (bitTime < _BIT_HALF_TIME) {
                _SET(actByte, 7);
            } else {
                _CLEAR(actByte, 7);
            }
        }
    }
    if (millis() > tillTime) {
        SPrintln("\r\nError: No signal, abort");
        return false;
    }
    return true;

}   // wait4Sync()


//------------------------------------------------------
bool catchByte(byte toCatch) {
    uint32_t    tillTime = millis() + 5000;
    int8_t      bit = 7;

    SPrint("Wait for "); printByte(toCatch); SPrintf(" (%c)",toCatch); 
    
    while (millis() < tillTime) 
    {
        yield();
        if (actByte == toCatch) 
        {
            SPrintln(" found!");
            return true;
        }
        for (bit = 0; bit <= 7; bit++) 
        {
            tillTime = millis() + 1000;
            actByte >>= (1);
            // wait for TAPE_IN to get High
            if (!wait4Pin2Get(HIGH, 100)) 
            {
                SPrintln("Signal lost?");
                return false;
            }
            bitStartTime    = micros(); 
            // wait for TAPE_IN to get Low
            if (!wait4Pin2Get(LOW, 100)) 
            {
                SPrintln("Signal lost?");
                return false;
            }
            bitTime         = micros() - bitStartTime;

            //printByte(actByte); SPrint(" -> ");
            if (bitTime < _BIT_HALF_TIME) 
            {
                _SET(actByte, 7);
            } else {
                _CLEAR(actByte, 7);
            }
        }
    }
    printByte(actByte); SPrint("  "); 
    if (millis() > tillTime) 
    {
        SPrintln("\r\ntimed out!");
        return false;
    }
    
    SPrintln();
    return true;

}   // catchByte()


//------------------------------------------------------
bool catchData(File dataFile) {
    uint32_t    tillTime = millis() + 1000;
    int8_t      bit;

    yield();
    for (bit = 0; bit <= 7; bit++) {
        actByte >>= (1);
        // wait for TAPE_IN to get High
        if (!wait4Pin2Get(HIGH, 100)) {
            SPrintln("Signal lost?");
            return false;
        }
        bitStartTime    = micros(); 
        // wait for TAPE_IN to get Low
        if (!wait4Pin2Get(LOW, 100)) {
            SPrintln("Signal lost?");
            return false;
        }
        bitTime         = micros() - bitStartTime;

        if (bitTime < _BIT_HALF_TIME) {
           _SET(actByte, 7);
        } else {
            _CLEAR(actByte, 7);
        }
    }
    dataFile.print((char)actByte);

    return true;
    
}   // catchData()


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
    
    tmpB = 0;
    
    getTimingDuration = millis() + 15000;

    SPrintln("\r\nWait for input signal ..");
    displayMsg("Wait for Signal ..");
    if (!wait4Pin2Get(LOW, 15000)) 
    {
        SPrintln("Error: no input signal found");
        displayMsgAndWait("Err: No Signal", 2);
        return;
    }
    if (!wait4Pin2Get(HIGH, 15000)) 
    {
        SPrintln("Error: no input signal found");
        displayMsgAndWait("Err: No Signal", 2);
        return;
    }

    displayMsg("Recording ...");

    SPrintln("\r\nWait for Sync ..");
    //---- get SYNC ------------
    if (!wait4Sync()) 
    {
        return;
    }
        
    SPrintln("\r\nRecording ..");
    //----- wait for '*' --------
    //catchByte(0b01010100);
    if (!catchByte(0x2A)) 
    {
        return;
    }
        
    //----- get Data --------
    //SPrintln("reading data .. ");     // this takes to long!
    //displayMsg("Read data ..");       // this takes to long
    tmpB        = 0;
    getTimingDuration = millis() + 500;
    while (millis() < getTimingDuration) 
    {
        while (actByte != '/') 
        {
            getTimingDuration = millis() + 500;
            if (!catchData(dataFile)) 
            {
                SPrintln("Error: lost signal from Tape..");
                displayMsgAndWait("Err: lost signal", 2);
                return;
            }
            
            if (actByte != '/') 
            {
                if (actByte >= ' ' && actByte <= '}') 
                {
                    SPrintf("%c",(char)actByte);
                } else 
                {
                    SPrint("?");
                }
                if (++tmpB%2 == 0) SPrint(" ");
                //if (tmpB%32  == 0) SPrintln();
            }
        }
        getTimingDuration = 0;
    }
    SPrintln();

    //----- get Data --------
    tmpB    = 0;
    //SPrintln("reading checkSum .. ");
    //displayMsg("Read Checksum ..");
    getTimingDuration = millis() + 500;
    while (millis() < getTimingDuration) 
    {
        while (actByte != 0x04) {   // EOT
            getTimingDuration = millis() + 500;
            if (!catchData(dataFile)) 
            {
                SPrintln("Error: lost signal from Tape..");
                displayMsgAndWait("Err: lost signal", 2);
                return;
            }
            if (actByte != 0x04) 
            {
                if (actByte >= ' ' && actByte <= '}')       
                        {SPrintf("%c",(char)actByte);}
                else    {SPrintf("?");}
                if (++tmpB%2 == 0) SPrint(" ");
                //if (tmpB%32  == 0) SPrintln();
            }
        }
        getTimingDuration = 0;
    }

    dataFile.close();

    if (String(progDetails.Name) == "<EmptySlot>") 
    {
      snprintf(progDetails.Name, sizeof(progDetails.Name), "Program-%s", progDetails.ID);
    }
    snprintf(idDir, sizeof(idDir), "%s", progDetails.ID);
    LittleFS.mkdir(idDir);
    sprintf(filePath, "/%s/%s.hex", progDetails.ID, progDetails.Name);

    SPrintf("\r\nremove [%s]\r\n", filePath);
    LittleFS.remove(filePath);
    SPrintf("rename [%s] to [%s] .. ", tempFile, filePath);
    if (LittleFS.rename(tempFile, filePath))
          SPrintln("");
    else  SPrintln("Error!");

    actFileID = progDetails.numID;
    readProgDetailsByID(actFileID);
    
    SPrintln("\r\n\nDone..");

}   // recordTape()
