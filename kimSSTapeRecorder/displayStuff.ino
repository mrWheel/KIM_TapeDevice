/*
***************************************************************************  
 *  Part of : uTapeEmulator
 *  Copyright (c) 2021 Willem Aandewiel
 *
 *  TERMS OF USE: MIT License. See bottom of file.  
 * 
 *    0,0 +--------------------------+ 128,0
 *        |                          |
 *        |                          |
 *        |                          |
 *        |                          |
 *   0,64 +--------------------------+ 128,64
 * 
 */

#include "SSD1306fonts.h"

//------------------------------------------------------
//-- clear the display
//-- sets:
//--   displayIsOff to true
//------------------------------------------------------
void turnOffOledDisplay() 
{
    display.clear();
    display.display();
    display.displayOff();
    displayIsOff = true;
    
}   // turnOffOledDisplay()


//------------------------------------------------------
//-- display's some text
//-- sets:
//--   displayIsOff to false
//------------------------------------------------------
void turnOnOledDisplay() 
{
    display.init();
    display.flipScreenVertically();
    display.display();
    //display.setFont(ArialMT_Plain_10);

    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);      // _24
    display.drawString(0, 0, "micro KIM");
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_16);
    display.drawString(64, 12, "Solid State");
    display.drawString(64, 31, "Tape Device");
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);      // _16/_24
    display.drawString(0, 52, "(c) Willem Aandewiel");
    display.display();
    displayIsOff = false;
    delay(1000);
    dimOledTimer = millis();

}   // turnOnOledDisplay()


//------------------------------------------------------
//-- shows a message on the display and wait for
//-- "wait" seconds 
//------------------------------------------------------
void displayMsgAndWait(String Msg, uint8_t wait) 
{
    String wOut[4];
    
    yield();
    sprintf(scrollName, Msg.c_str());
    int wc = splitString(Msg, '\n', wOut, 4);
    for(int i=0; i<wc; i++)
    {
      DebugTf("[%d] -> [%s]\r\n", i, wOut[i].c_str());
    }
    display.setColor(BLACK);
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setColor(WHITE);
    for (int i=0; i<wc; i++)
    {
      display.drawString(0, (18*i),  wOut[i]);
    }
    display.display();
    if (wait) 
    {
        delay(wait * 1000);
    }
    
}   // displayMsgAndWait()


//------------------------------------------------------
//-- shows a message on the display
//------------------------------------------------------
void displayMsg(String Msg) 
{
    displayMsgAndWait(Msg, 0);
    
}   // displayMsg()


//------------------------------------------------------
//-- clears line "line" of the display
//-- shows a message at line "line" {0-3}
//-- and waits "wait" seconds
//------------------------------------------------------
void displayMsg(uint8_t line, String Msg, uint8_t wait) 
{
  display.setColor(BLACK);
  display.setFont(ArialMT_Plain_16);
  if (line == 0)
  {
    display.clear();
  }
  else
  {
    display.fillRect(0, (18*line), 128,  (18*line)+17);
  }
  display.setColor(WHITE);
  display.drawString(0, (18*line),  Msg);
  display.display();
  if (wait) 
  {
    delay(wait * 1000);
  }
    
}   // displayMsg()


//-------------------------------------------------------
//-- display's various fields from the progDetails struct
//-------------------------------------------------------
void displayProgDetails() 
{
    String tmpString;

    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setColor(WHITE);
    display.drawString(5, 5, progDetails.ID);
    //display.drawRect(0, 0, 36, 36);
    display.drawRect(0, 0, 45, 36);

    display.setFont(ArialMT_Plain_24);
    if (progDetails.Lock) 
    {
        display.drawString(60, 2, "*");
    }
    //display.setFont(ArialMT_Plain_16);
    display.setFont(Monospaced_plain_16);
    tmpString = "$" + String(progDetails.Strt);
    display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)),  0, tmpString);
    tmpString = "$" + String(progDetails.End);
    display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 20, tmpString);

    display.display();
    idxName = 0;
    
}   // displayProgDetails()


//------------------------------------------------------
//-- scroll's the program name from right to left
//------------------------------------------------------
void scrollProgName() 
{
    int16_t pBuff;

    String sBuff = String(progDetails.Name).c_str();
    if (sBuff.length() < 16) 
    {
        display.setColor(WHITE);
        display.drawString(0, 45, String(sBuff));
        display.display();
        return;
    }
    
    display.setColor(BLACK);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 45, String(scrollName));
    sBuff.concat("       ");
    if (idxName > sBuff.length()) 
    {
        idxName = 0;
        return;
    }
    pBuff = idxName++;
    for (int p=0; p<16; p++) 
    {
        yield();
        if (sBuff[pBuff] == 0) 
        {
            pBuff   = 0;
            scrollName[p] = ' ';
        } else {
            scrollName[p] = sBuff[pBuff++];
        }
    }
    scrollName[16] = 0;
    display.setColor(WHITE);
    display.drawString(0, 45, String(scrollName));
    display.display();
    
}   // scrollProgName()


//------------------------------------------------------
//-- scroll's the tape progress bar
//------------------------------------------------------
void scrollProgressTape(bool up) 
{
    volatile int sdPos;
    char savC;

    display.setColor(BLACK);
    display.setFont(ArialMT_Plain_24);
    display.fillRect(0, 38, DISPLAY_WIDTH, 38);
    display.display();


    if (up)
    {
      savC = scrollName[strlen(scrollName)-1];
      for(int p=(strlen(scrollName)-1); p>0; p--)
      {
        scrollName[p] = scrollName[p-1];
      }
      scrollName[0] = savC;
    }
    else
    {
      savC = scrollName[0];
      for(int p=0; p<(strlen(scrollName)-1); p++)
      {
        scrollName[p] = scrollName[p+1];
      }
      scrollName[strlen(scrollName)-1] = savC;
    }
    scrollName[strlen(scrollName)] = 0;
    
    display.setColor(WHITE);
    display.drawString(0, 45, String(scrollName));
    display.display();
    
}   // scrollProgressTape()


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
