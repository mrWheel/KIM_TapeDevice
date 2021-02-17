/*
***************************************************************************  
**  Program  : ATtinyPLL2
**  Copyright (c) 2017-2021 Willem Aandewiel
 */
#define _FW_VERSION "v2.0.0 (16-02-2021)"
/* 
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************  
**  Arduino IDE settings:  
* * ---------------------
**    Chip: "ATtyni841"
**    Clock: "8MHz (internal, Vcc > 4.5V)"
**    B.O.D. Mode (active): "B.O.D. Disabled (saves power)"
**    Pin Mapping: "Clockwise (like Rev. D/E boards)"
**    LTO (1.6.11+ only): "Enabled"
**    Bootloader UART: "UART0 (Serial)"
**    Wire Modes: "Master Only";
**    millis()/micros(): "Enabled"
** 
***************************************************************************
** 
** Micro-KIM Tape Device, ATTiny841 / ESP8266
**
** The original KIM-1 uses a PLL and a comparator to implement the receive
** path of the cassette interface. These two parts have been replaced by
** this implementation which runs on a ATTiny85 microcontroller.
**
** The KIM-1 cassette format encodes each bit using two tones:
**  1) A high-frequency tone of 3623.188 Hz 
**  2) A low-frequency tone of 2415.459 Hz
** 
** Each bit is broken up into three periods, each of which is 2.484 ms long,
** during which either the low frequency or the high frequency tone is played.
**
** Every bit starts with some high frequency pulses and ends with some low 
** frequency pulses. 
** The moment of the transition between the high- and low-frequency determines
** whether it is a '1' or a '0' bit. 
**
**      <-2482ms-> <-2484ms-> <-2484ms->
**      HHHHHHHHH  HHHHHHHHH    LLLLLL              => is a '0'
**      HHHHHHHHH    LLLLLL     LLLLLL              => is a '1'
**     
**      <---- duration ------>
**      <-duration-->
**
** The KIM-1 cassette interface uses a PLL tuned to distinguish between the
** high and low frequency tone. The output of this PLL is then fed into a
** comparator to generate a logic '1' whenever the high frequency is detected,
** and a logic '0' whenever the low frequency is detected. This logic signal
** is analyzed by the KIM-1 to reconstruct the bit-stream stored on the cassette
** tape. Each bit begins with a low-high transition, and the bit value can be
** determine by the timing of the falling edge generated by the high-frequency
** to low-frequency transition within each bit.
** 
** The job of the ATTiny841 is to perform the same function of the original PLL 
** and comparator: analyze the input signal, and generate a logic '1' output 
** whenever the high frequency is detected, and a logic '0' output whenever the 
** low frequency is detected. 
** It does this by starting a timer the moment the High freguency is detected
** and stop the timer the moment the Low frequency is detected. If the duration 
** between the starting and ending is > 3.726 milliseconds, than make the 
** output-pin LOW. If it is < 3.726 millis seconds, than make the output-pin
** HIGH.
** 
** Bits are shifted into a byte and, if that byte represents a valid char 
** that char is send by the output-pin to the TAPE_IN pin of the ESP8266 for 
** further processing.
**
** In fact: at first the bits are shifted into the byte and checked against 
** a SYNC char. If it is a SYNC char, from then on, it will build a new char 
** every 8 bits.
**   
***************************************************************************  
*/

#include "Debug.h"

#define _BV(bit)             (1 << (bit))
#define _SET(a,b)            ((a) |= _BV(b))
#define _CLEAR(a,b)          ((a) &= ~_BV(b))

//----- define _DEBUG as 1 or 0 (1 enables DPrint macro's)
#define _DEBUG  0

// ======================================================
// determine CPY type by verbose compile (preferences)
// parm -mmcu=atxys ==> transaltes into "__AVR_ATxys__"
// Also the -D flag can have/has someting to do with it..
// https://stackoverflow.com/questions/13853109/determine-board-type-of-arduino
// ======================================================
#if defined(__AVR_ATmega328P__)
    #define _IN_FROM_KIM     3          // ArduinoPin D3
    #define _PLL_TX          7          // ArduinoPin 7
    //
    #define _MIDFREQ            340
    #define _MIDTRAINDURATION  3700
    //
//
#elif defined(__AVR_ATtiny841__)
//                            +----V----+
//                            |VCC   GND|
//                            |PB0   PA0|
//     _IN_FROM_KIM --> INT0 -|PB1   PA1|- TxD --> _PLL_TX (Serial)
//                    /RESET -|PB3   PA2|- RxD <-- _PLL_RX (not used)
//                            |PB2   PA3|
//                   LED <-- -|PA7   PA4|- SCK
//                      MOSI -|PA6   PA5|- MISO
//                            +---------+
//
    #define _IN_FROM_KIM    PIN_PB1     // DIL-3 / ArduinoPin D9
    #define _PLL_TX         PIN_PA1     // DIL-12 / Serial TX
    #define _PLL_RX         PIN_PA2     // DIL-11 / Serial RX
    #define _PLL_LED        PIN_PA7
    //
    #define _MIDFREQ            340
    #define _MIDTRAINDURATION  3700
    //
    //
    #ifdef PINMAPPING_CCW
      #error "PINMAPPING ERROR: Sketch was written for clockwise pin mapping!"
    #endif
    //
#else
    #error "BOARD ERROR: Choose Arduino UNO or ATtiny841!"
#endif

enum    { ISR_INIT, ISR_TUNE, ISR_SYNC, ISR_START, ISR_DATA };
enum    { BIT_START, BIT_END };

char    ascii[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

//-------- ALL ISR variables -----------------------------------------------
volatile uint32_t ISRstartPulseTime, ISRprevPulseTime, ISRthisPulseDuration;
volatile uint32_t ISRstartBitTime;
volatile uint32_t ISRstartTrainTime, ISRthisTrainDuration;
volatile byte     ISRstate;
volatile char     ISRchar, ISRbyte;
volatile uint8_t  ISRbitCount;
volatile int32_t  ISRbyteCount;
volatile byte     ISRbitState;
volatile bool     ISRhasChar;

uint32_t  midFrequency;        // use this pulseDuration as base for the long/short test
uint32_t  midTrainDuration;
uint32_t  inactiveTime;     // if Due, start measering the pulseWidt at firs pulse
uint32_t  trainDuration = 0;
uint32_t  trainCount = 0;
uint16_t  charCounter = 0;
uint16_t  duration3700Hz;
uint16_t  duration2400Hz;
bool      tunePLL = true;
char      hexByte[6];


//========================================================================================
//--  pulse @3700Hz => 270uSec
//--  pulse @2400Hz => 417uSec 
//--  The time between two LOW-to-HIGH transitions is meassered
//--  If the time changed betweed two consecutive transitions
//--  we have a new bit (either '1' or '0').
//--
//-- <--------7425uSec-------->
//-- <-------4968------> <2467>
//-- HHHHHHHHH HHHHHHHHH LLLLLL == "0"
//-- <--2484-> <-----4968----->
//-- HHHHHHHHH LLLLLL    LLLLLL == "1"
//-- <----3712---->      midTrainDuration
//--
//-- A bit starts with 9 or 18 pulses with a duration of ~270ms
//-- If, after 9 pulses (~2480ms) the duration changes 
//-- from ~270ms to ~410ms we have a '1'. If the change occurs 
//-- after 18 pulses (~4960ms) we have '0'.
//-- In fact: we check whether the change takes place before
//-- ~3700ms (it's a '1') or after ~3700ms (it's a '0').
//========================================================================================
void ISRgetByte()
{
  if (!digitalRead(_IN_FROM_KIM)) return;
    switch(ISRbitState)
    {
      case BIT_START:
            ISRprevPulseTime      = ISRthisPulseDuration;
            ISRthisPulseDuration  = micros() - ISRstartPulseTime;
            ISRstartPulseTime     = micros();    
            //
            //   +----+    +----+    +--+  +--+
            //   |    |    |    |    |  |  |  |
            // --+    +----+    +----+  +--+  +--   
            //                          ^
            //                          +--- ISRstartBitTime  
            //  
            //--- change from 2400Hz to 3700Hz is start of bit
            if ((ISRthisPulseDuration > midFrequency)) 
            {
              ISRstartBitTime       = micros();
              ISRthisTrainDuration  = micros() - ISRstartTrainTime;
              ISRstartTrainTime     = micros();
              ISRbitState           = BIT_END;
            }
            return;
            break;
            
      case BIT_END:
            ISRthisPulseDuration    = micros() - ISRstartPulseTime;
            ISRstartPulseTime       = micros();
            //
            //   +--+  +--+  +----+    +----+
            //   |  |  |  |  |    |    |    |
            // --+  +--+  +--+    +----+    +--   
            //                    ^
            //                    +--- endBitTime    
            //
            //--- first change from 3700Hz to 2400Hz is end of bit
            if ((ISRthisPulseDuration < midFrequency)) 
            {                   //              7654 3210 <- bitNr
              ISRbyte >>= (1);  // shift right: 0101 0100 => 0010 1010 
              if ((micros() - ISRstartBitTime) > midTrainDuration)
              {
                _SET(ISRbyte, 7); // new bit (1) at the beginning
              }
              else
              {
                _CLEAR(ISRbyte, 7); // new bit (0) at the beginning
              }
              ISRbitCount++;
              ISRbitState = BIT_START;
            }
            break;

      default:  ISRbitState = BIT_START;
    
    } //  switch(ISRbitState) ..

    switch(ISRstate)
    {
      case ISR_INIT:  
            memcpy(ISRbyte, 0, sizeof(ISRbyte));
            ISRbitCount   = 0;
            ISRbyteCount  = 0;
            ISRhasChar    = false;
            ISRstate      = ISR_SYNC;
            break;

      case ISR_SYNC:  
            if (ISRbyte == 0x16)  // 0001 0110 = "SYNC" 
            {
              SPrintln();
              ISRhasChar  = true;
              ISRchar     = (char)ISRbyte;
              ISRbitCount = 0;
              ISRbyteCount++;
              ISRstate    = ISR_START;
            }
            break;

      case ISR_START:  
            if (ISRbitCount > 7)
            {
              ISRhasChar  = true;
              ISRbitCount = 0;
              ISRchar     = (char)ISRbyte;
              if (ISRchar == 0b00101010)  // 00101010 = '*' 
              {
                ISRbyteCount = 0;
                ISRstate = ISR_DATA;
              }
            }
            break;

      case ISR_DATA:  
            if (ISRbitCount > 7)
            {
              ISRhasChar  = true;
              ISRbitCount = 0;
              ISRchar     = (char)ISRbyte;
              if (ISRchar == '/') 
              {
                ISRbyteCount = 0;
              }
            }
            break;

    } //  switch(ISRstate) ..
    
} //  ISRgetByte()


//========================================================================================
// DPrints out Binary value (1 or 0) of byte
//------------------------------------------------------
void printByte(int c) 
{
    for (int bit = 7; bit >= 0; bit--) 
    {
        // Compare bit 7-0 in byte
        if (c & (1 << bit)) 
        {
            DPrint ("1");
        } 
        else 
        {
            DPrint ("0");
        }
    }

}   // printByte()


//========================================================================================
//-- Skipping the first 3 SYNC chars we measeure the total time of the
//-- next 17 SYNC chars. Ths time devided by 17 gives us the time
//-- for a pulse trainDuration.
//-- With this (tuned) trainDuration we calculate the 3700Hz and
//-- 2400Hz pulse durations and the midFrequency.
//----------------------------------------------------------------------------------------
void tunePLLtiming() 
{
  if (trainCount < 20)  // 20 SYNC chars should be enough!
  {
    trainCount++;
    if (trainCount > 3)
    {
      DPrint(trainCount); DPrint("T"); DPrintln(ISRthisTrainDuration);
      trainDuration += ISRthisTrainDuration;
    }
  }
  else
  {
    trainCount -= 3;
    DPrintln();
    trainDuration = trainDuration / trainCount;
    DPrint("trainDuration["); DPrint(trainDuration);
    DPrintln("]");
    midTrainDuration = trainDuration / 2;
    DPrint("calculated midTrainDuration["); DPrint(midTrainDuration);
    DPrintln("]");
    duration3700Hz = trainDuration / 27;
    duration2400Hz = trainDuration / 18;
    midFrequency = (duration3700Hz + duration2400Hz) / 2;
    DPrint("midFrequency["); DPrint(midFrequency);
    DPrintln("]");
    tunePLL = false;
    ISRbyteCount = 0;
  }

} //  tunePLLtiming()


//========================================================================================
void setup() 
{
    pinMode(_IN_FROM_KIM, INPUT); 
    pinMode(_PLL_TX,      OUTPUT);
    pinMode(PIN_PA0,      OUTPUT);
    pinMode(_PLL_LED,     OUTPUT);

    Serial.begin(19200);
    while(!Serial) { delay(100); }

    for(int i=0; i<6; i++)
    {
      delay(100);
      DPrintln("\r\n===== ATtinyPLL2 =====");
      digitalWrite(_PLL_LED, !digitalRead(_PLL_LED));
    }
    
    DPrint("IN FROM KIM : "); DPrintln(_IN_FROM_KIM);  
    DPrint("TX TO ESP   : "); DPrintln(_PLL_TX);
    DPrint("_PLL_LED    : "); DPrintln(_PLL_LED);

    ISRstate          = ISR_INIT;
    ISRbitState       = BIT_START;
    tunePLL           = true;
    inactiveTime      = millis();
    midFrequency      = _MIDFREQ;
    midTrainDuration  = _MIDTRAINDURATION;
    digitalWrite(_PLL_LED, LOW);
    
    attachInterrupt(digitalPinToInterrupt(_IN_FROM_KIM), ISRgetByte, RISING);
    
}   // setup()


//========================================================================================
void loop() 
{
    if ((millis() - inactiveTime) > 1500)
    {
      inactiveTime  = millis();
      ISRstate      = ISR_INIT;
      digitalWrite(_PLL_LED, LOW);
      ISRhasChar    = false;
      ISRbyteCount  = 0;
      tunePLL       = true;
      trainCount    = 0;
      trainDuration = 0;
      if (++charCounter%20 == 0)  DPrintln(".");
      else                        DPrint(".");
    }
    
    if (ISRhasChar)
    {
      inactiveTime = millis();
      ISRhasChar = false; 
      
      if (ISRchar == 0x16 )
      {
        if (tunePLL)
        {
          digitalWrite(_PLL_LED, HIGH);
          tunePLLtiming();
          digitalWrite(_PLL_LED, LOW);
        }
        if (++charCounter%2 == 0) SPrint("S");
        return; // skip the rest
      }
      
      if (ISRbyteCount%2  == 0)  DPrint(" ");
      if (ISRbyteCount%32 == 0) 
      {
        SPrintln();
        sprintf(hexByte, "%04x", ISRbyteCount);
        DPrint(hexByte);
        DPrint(" - "); 
      }
      
      if (ISRchar == 0x16)
      {
        SPrint("S");
      } else
      {
        SPrint((char)ISRchar);  // comunicate with the ESP8266
      }
      //SFlush();
          
      if (ISRchar == '*')
      {
        DPrint(" ");        // just for the debug lay-out 
        digitalWrite(_PLL_LED, HIGH);
        ISRbyteCount = -7;  // just for the debug lay-out
      }
      else if (ISRchar == '/')
      {
        DPrint(" ");        // just for the debug lay-out
        ISRbyteCount = 1;   // just for the debug lay-out
      }
      else if (ISRchar == 0x04)
      {
        DPrint("EOT");      // just for the debug lay-out
        ISRbyteCount = 1;   // just for the debug lay-out
        digitalWrite(_PLL_LED, LOW);
      }
      
      ISRbyteCount++;
    }

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
