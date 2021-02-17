/*
***************************************************************************  
**  Program  : kimSSTapeRecorder v2.0 / Playback
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/

#define _SHORT   200      // 300 is more-or-less standaard (minimal 190?)
#define _LONG    (_SHORT * 2)


//----------------------------------------------------------
//-- converts an ascii character into a byte value
//--    ascii--decimal--binary --->  byte--decimal--binary
//--     "0"     48    0110000 --->           0      0000
//--     "6"     54    0110110 --->           6      0110 
//--     "B"     66    1000010 --->          11      1011
//--     "F"     70    1000110 --->          16      1111
//----------------------------------------------------------
byte char2Byte(char cIn) 
{
    byte iByte;

    if (cIn >= '0' && cIn <= '9') 
    {
        iByte = (cIn - '0') & 0xF;
    } else  
    {
        iByte = ((cIn - '0')-7) & 0xF;
    }

    return iByte;

}   // char2Byte()


//------------------------------------------------------
//-- send one bit to _TAPE_OUT_PIN
//-- if bit is 1 sends HHHHLLLLLLLL
//-- if bit is 0 sends HHHHHHHHLLLL
//------------------------------------------------------
void sendOneBit(boolean cBit) 
{
#if (_REAL_KIM1 == true)
    if (cBit) 
    {
        digitalWrite(_TAPE_OUT_PIN, LOW);
        delayMicroseconds(_SHORT);
        digitalWrite(_TAPE_OUT_PIN, HIGH);
        delayMicroseconds(_LONG);
        
    } else 
    {
        digitalWrite(_TAPE_OUT_PIN, LOW);
        delayMicroseconds(_LONG);
        digitalWrite(_TAPE_OUT_PIN, HIGH);
        delayMicroseconds(_SHORT);
    }
#else
    if (cBit) 
    {
        digitalWrite(_TAPE_OUT_PIN, HIGH);
        delayMicroseconds(_SHORT);
        digitalWrite(_TAPE_OUT_PIN, LOW);
        delayMicroseconds(_LONG);
        
    } else 
    {
        digitalWrite(_TAPE_OUT_PIN, HIGH);
        delayMicroseconds(_LONG);
        digitalWrite(_TAPE_OUT_PIN, LOW);
        delayMicroseconds(_SHORT);
    }

#endif
} // sendOneBit()


//------------------------------------------------------
//-- splits a byte into bits and send al the bits
//-- until the whole byte is send
//------------------------------------------------------
void sendOneByte(byte cByte) 
{
    uint8_t mask;
       
    for (int b=0; b<=7; b++) 
    {
        mask = 1;
        mask <<= (b);
        if (cByte & mask) 
        {
            sendOneBit(1);
        } else 
        {
            sendOneBit(0);
        }
    }
    
}   // sendOneByte()


//------------------------------------------------------
//-- Splits a byte (8 bits) in two nibbles (4 bit)
//-- and sends the nibbles high-nibble first
//-- so byte: 0101 1011 
//-- will be converted into:
//--      HighByte 0000 0101 (dec)5  -> (ascii)53 
//--           and send to _TAPE_OUT_PIN as 00110101
//-- and  LowByte  0000 1011 (dec)11 -> (ascii)66 
//--           and send to _TAPE_OUT_PIN as 01000010
//------------------------------------------------------
void sendTwoAscii(byte cByte) 
{
    uint8_t mask;
       
    lByte =  cByte & 0xF;
    hByte = (cByte >> 4) & 0xF;
    
    for (int b=0; b<=7; b++) 
    {
        mask = 1;
        mask <<= (b);
        if (ascii[hByte] & mask) 
        {
            sendOneBit(1);
        } else 
        {
            sendOneBit(0);
        }
    }
    for (int b=0; b<=7; b++) 
    {
        mask = 1;
        mask <<= (b);
        if (ascii[lByte] & mask) 
        {
            sendOneBit(1);
        } else 
        {
            sendOneBit(0);
        }
    }
    
}   // sendTwoAscii()


//------------------------------------------------------
//-- reads the ".hex" program file in the active Slot
//-- and sends it to _TAPE_IN_PIN
//-- First sends SYNC (0x16) char for 3 seconds
//-- Second sends a "*" (0x2A)
//-- Then sens all the bytes in the file
//------------------------------------------------------
void playbackTape() 
{    
  SPrint("\r\nPlaying Tape ");
  sendStatusMessage("Playing Tape ..");

  displayMsg("Playing ...");

  sprintf(filePath, "/%s/%s.hex", progDetails.ID, progDetails.Name);
  SPrintf("[%s]\r\n\n", filePath);
  File dataFile = LittleFS.open(filePath, "r");

  if (!dataFile)
  {
    snprintf(wsSend, sizeof(wsSend), "Error: [%s.hex] not found!!", progDetails.Name);
    sendStatusMessage(wsSend);
    SPrintln(wsSend);
    return;
  }

  pinMode(_TAPE_OUT_PIN, OUTPUT);

  getTimingDuration = millis() + 3000;
  //SPrintln("Send SYNC (0x16) for 3 seconds");
  snprintf(wsSend, sizeof(wsSend), "Send SYNC (0x16) for 3 seconds\r");
  SPrintln("Send SYNC (0x16) for 3 seconds\r");
  sendStatusMessage("Send SYNC (0x16) for 3 seconds");
  while (millis() < getTimingDuration) 
  {
    yield();
    // SYNC 0x16
    // 0b00010110
    sendOneByte(0x16);
  }

  sendStatusMessage("Send '*' (0x2A)");
  SPrintln("Send '*' (0x2A)\r");
  sendOneByte('*');
  SPrintln("---------------------------------\r");

  sendStatusMessage("Send programData");
  SPrintln("Send programData\r");

  tmpB = 12;
  while (dataFile.available()) 
  {
    yield();
    hByte = (char)dataFile.read();
    // skip non-printable chars ---------------
    if (hByte <= ' ' || hByte >= '}') continue;
    if ((++tmpB%8) == 0)  { SPrint(" ");    } 
    if ((tmpB%16)  == 0)  { SPrintln("\r"); }

    if (   (hByte >= '0' && hByte <= '9') 
        || (hByte >= 'A' && hByte <= 'F') ) 
    {
      actByte = char2Byte(hByte) * 0x10;
      if (dataFile.available()) {
        lByte = (char)dataFile.read();
        actByte += char2Byte(lByte);
      }
      SPrint(" ");
      if (actByte < 0x10) 
      {
        SPrint("0"); 
      }
                
      SPrint(actByte, HEX);  
      sendTwoAscii(actByte);
        
    } else   // not in 0-9, A-F
    {
      actByte = hByte;
      if (actByte == '/') { SPrint("\r\n"); tmpB = 0; }          
      if (actByte < 0x10) { SPrint("0"); }
      SPrint(actByte, HEX);  
      SPrintf(" (%c) ", actByte);         
      sendOneByte(actByte);
    }
            
  }   // while there is data

  SPrintln("\r\n---------------------------------");

  displayMsg("Send EOT (0x04)");
  SPrint("Send EOT (0x04) 2 times ..");
  sendStatusMessage("Send EOT (0x04) 2 times ..");
  for (int L=0; L<2; L++) 
  {
    // EOT 0x04
    // 0b00000100
    sendOneByte(0x04);
  }
  
  dataFile.close();
  SPrintln("\r\nDone..");

  pinMode(_TAPE_OUT_PIN, INPUT_PULLUP);
  
}   // playbackTape()

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
