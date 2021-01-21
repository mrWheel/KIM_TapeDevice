/*
***************************************************************************  
**  Program  : uTapeEmulator v2 / Playback
**  Copyright (c) 2017 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/

#define _SHORT   300           // Standaard is 2484 max is 8000
#define _LONG    (_SHORT * 2)


//------------------------------------------------------
byte char2Byte(char cIn) {
    byte iByte;

    if (cIn >= '0' && cIn <= '9') {
        iByte = (cIn - '0') & 0xF;
            
    } else  {
        iByte = ((cIn - '0')-7) & 0xF;
    }

    return iByte;

}   // char2Byte()


//------------------------------------------------------
void sendOneBit(boolean cBit) {

    if (cBit) {
        digitalWrite(_TAPE_OUT_PIN, HIGH);
        delayMicroseconds(_SHORT);
        digitalWrite(_TAPE_OUT_PIN, LOW);
        delayMicroseconds(_LONG);
        
    } else {
        digitalWrite(_TAPE_OUT_PIN, HIGH);
        delayMicroseconds(_LONG);
        digitalWrite(_TAPE_OUT_PIN, LOW);
        delayMicroseconds(_SHORT);
    }
  
} // sendOneBit()


//------------------------------------------------------
void sendOneByte(byte cByte) {
    uint8_t mask;
       
    for (int b=0; b<=7; b++) {
        mask = 1;
        mask <<= (b);
        if (cByte & mask) {
            sendOneBit(1);
        } else {
            sendOneBit(0);
        }
    }
    
}   // sendOneByte()


//------------------------------------------------------
void sendTwoAscii(byte cByte) {
    uint8_t mask;
       
    lByte =  cByte & 0xF;
    hByte = (cByte >> 4) & 0xF;
    
    for (int b=0; b<=7; b++) {
        mask = 1;
        mask <<= (b);
        if (ascii[hByte] & mask) {
            sendOneBit(1);
        } else {
            sendOneBit(0);
        }
    }
    for (int b=0; b<=7; b++) {
        mask = 1;
        mask <<= (b);
        if (ascii[lByte] & mask) {
            sendOneBit(1);
        } else {
            sendOneBit(0);
        }
    }
    
}   // sendTwoAscii()


//------------------------------------------------------
void playbackTape() 
{    
  SPrint("\r\nPlaying Tape ");
  displayMsg("Playing ...");

  sprintf(filePath, "/%s/%s.hex", progDetails.ID, progDetails.Name);
  SPrintf("[%s]\r\n\n", filePath);
  File dataFile = LittleFS.open(filePath, "r");

  if (!dataFile)
  {
    SPrintln("Error: File not found!!");
    return;
  }

  getTimingDuration = millis() + 3000;
  SPrintln("Send SYNC (0x16) for 3 seconds");
  while (millis() < getTimingDuration) 
  {
    yield();
    // SYNC 0x16
    // 0b00010110
    sendOneByte(0x16);
  }

  SPrint("Send '*' (0x2A)");
  sendOneByte('*');
  SPrintln("\r\n---------------------------------");

  tmpB = 0;
  while (dataFile.available()) 
  {
        yield();
        hByte = (char)dataFile.read();
        // skip non-printable chars ---------------
        if (hByte <= ' ' || hByte >= '}') continue;
        if ((tmpB%16) == 0)     {SPrintln();} 
        else if ((tmpB%8) == 0) {SPrint(" ");}

        tmpB++;
        if (   (hByte >= '0' && hByte <= '9') 
            || (hByte >= 'A' && hByte <= 'F') ) {
                actByte = char2Byte(hByte) * 0x10;
                if (dataFile.available()) {
                    lByte = (char)dataFile.read();
                    actByte += char2Byte(lByte);
                }
                SPrint(" ");
                if (actByte < 0x10) {
                    SPrint("0"); 
                }
                
                SPrint(actByte, HEX);  
                sendTwoAscii(actByte);
        
        } else {    // not in 0-9, A-F
                actByte = hByte;
                SPrint(" >> ");            
                if (actByte < 0x10) SPrint("0"); 
                SPrint(actByte, HEX);  
                SPrintf(" (%c)\r\n", actByte);         
                sendOneByte(actByte);

        }
            
  }   // while there is data

  SPrintln("\r\n---------------------------------");

  //displayMsg("Send EOT (0x04)");
  SPrint("Send EOT (0x04) 2 times ..");
  for (int L=0; L<2; L++) 
  {
    // EOT 0x04
    // 0b00000100
    sendOneByte(0x04);
  }

  dataFile.close();
  SPrintln("\r\nDone..");

}   // playbackTape()
