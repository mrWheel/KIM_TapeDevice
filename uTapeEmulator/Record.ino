/*
***************************************************************************  
**  Program  : uTapeEmulator v2 / Record
**  Copyright (c) 2017 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/

#define _BIT_HALF_TIME  3750


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


bool wait4Sync() {
    uint32_t    tillTime = millis() + 15000;

    Serial.print("Wait for "); printByte(0x16); 
    while (millis() < tillTime) {
        yield();
        if (actByte == 0x16) {
            Serial.println(" found!");
            return true;
        } else {
            yield();
            actByte >>= (1);
            // wait for TAPE_IN to get High
            if (!wait4Pin2Get(HIGH, 1000)) {
                Serial.print(".");
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
        Serial.println("\nError: No signal, abort");
        return false;
    }
    return true;

}   // wait4Sync()


bool catchByte(byte toCatch) {
    uint32_t    tillTime = millis() + 5000;
    int8_t      bit = 7;

    dPrint("Wait for "); printByte(toCatch); dPrintf(" (%c)",toCatch); 
    
    while (millis() < tillTime) {
        yield();
        if (actByte == toCatch) {
            dPrintln(" found!");
            return true;
        }
        for (bit = 0; bit <= 7; bit++) {
            tillTime = millis() + 1000;
            actByte >>= (1);
            // wait for TAPE_IN to get High
            if (!wait4Pin2Get(HIGH, 100)) {
                Serial.println("Signal lost?");
                return false;
            }
            bitStartTime    = micros(); 
            // wait for TAPE_IN to get Low
            if (!wait4Pin2Get(LOW, 100)) {
                Serial.println("Signal lost?");
                return false;
            }
            bitTime         = micros() - bitStartTime;

            //printByte(actByte); dPrint(" -> ");
            if (bitTime < _BIT_HALF_TIME) {
                _SET(actByte, 7);
            } else {
                _CLEAR(actByte, 7);
            }
        }
    }
    printByte(actByte); dPrint("  "); 
    if (millis() > tillTime) {
        Serial.println("\ntimed out!");
        return false;
    }
    
    dPrintln();
    return true;

}   // catchByte()


bool catchData(File dataFile) {
    uint32_t    tillTime = millis() + 1000;
    int8_t      bit;

    yield();
    for (bit = 0; bit <= 7; bit++) {
        actByte >>= (1);
        // wait for TAPE_IN to get High
        if (!wait4Pin2Get(HIGH, 100)) {
            Serial.println("Signal lost?");
            return false;
        }
        bitStartTime    = micros(); 
        // wait for TAPE_IN to get Low
        if (!wait4Pin2Get(LOW, 100)) {
            Serial.println("Signal lost?");
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


void recordTape() {

    Serial.println("\nSave program to cassette tape ..");

    if (progDetails.Lock) {
        Serial.println("Error: Program is locked (RO)!");
        displayErrorMsg("** ReadOnly **", 2);
        return;
    }
    sprintf(filePath, "%s/prgs/%s.hex", _DATAROOT, progDetails.ID);
    
    File dataFile = SPIFFS.open("/KIM/tmp.hex", "w");
    
    tmpB = 0;
    
    getTimingDuration = millis() + 15000;

    Serial.println("\nWait for input signal ..");
    displayMsg("Wait for Signal ..");
    if (!wait4Pin2Get(LOW, 15000)) {
        Serial.println("Error: no input signal found");
        displayErrorMsg("Err: No Signal", 2);
        return;
    }
    if (!wait4Pin2Get(HIGH, 15000)) {
        Serial.println("Error: no input signal found");
        displayErrorMsg("Err: No Signal", 2);
        return;
    }

    displayMsg("Recording ...");

    Serial.println("\nWait for Sync ..");
    //---- get SYNC ------------
    if (!wait4Sync()) {
        return;
    }
        
    Serial.println("\nRecording ..");
    //----- wait for '*' --------
    //catchByte(0b01010100);
    if (!catchByte(0x2A)) {
        return;
    }
        
    //----- get Data --------
    //dPrintln("reading data .. ");     // this takes to long!
    //displayMsg("Read data ..");       // this takes to long
    tmpB        = 0;
    getTimingDuration = millis() + 500;
    while (millis() < getTimingDuration) {
        while (actByte != '/') {
            getTimingDuration = millis() + 500;
            if (!catchData(dataFile)) {
                Serial.println("Error: lost signal from Tape..");
                displayErrorMsg("Err: lost signal", 2);
                return;
            }
            
            if (actByte != '/') {
                if (actByte >= ' ' && actByte <= '}') {
                    dPrintf("%c",(char)actByte);
                } else {
                    dPrint("?");
                }
                if (++tmpB%2 == 0) dPrint(" ");
                if (tmpB%32  == 0) dPrintln();
            }
        }
        getTimingDuration = 0;
    }
    dPrintln();

    //----- get Data --------
    tmpB    = 0;
    //dPrintln("reading checkSum .. ");
    //displayMsg("Read Checksum ..");
    getTimingDuration = millis() + 500;
    while (millis() < getTimingDuration) {
        while (actByte != 0x04) {   // EOT
            getTimingDuration = millis() + 500;
            if (!catchData(dataFile)) {
                Serial.println("Error: lost signal from Tape..");
                displayErrorMsg("Err: lost signal", 2);
                return;
            }
            if (actByte != 0x04) {
                if (actByte >= ' ' && actByte <= '}')       
                        {dPrintf("%c",(char)actByte);}
                else    {dPrintf("?");}
                if (++tmpB%2 == 0) dPrint(" ");
                if (tmpB%32  == 0) dPrintln();
            }
        }
        getTimingDuration = 0;
    }

    dataFile.close();
    dPrintf("\nremove [%s]\n", filePath);
    SPIFFS.remove(filePath);
    dPrintf("rename [/KIM/tmp.hex] to [%s]\n", filePath);
    SPIFFS.rename("/KIM/tmp.hex", filePath);
    
    Serial.println("\n\nDone..");

}   // recordTape()

