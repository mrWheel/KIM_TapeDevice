/*
 * 
 *   0,0  +--------------------------+ 128,0
 *        |                          |
 *        |                          |
 *        |                          |
 *        |                          |
 *  64,0  +--------------------------+  18,64
 * 
 */




void turnOffOledDisplay() {
    display.clear();
    display.display();
    displayIsOff = true;
    
}   // turnOffOledDisplay()


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
    delay(2000);
    dimOledTimer = millis();

}   // turnOnOledDisplay()


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


void displayMsg(String Msg) 
{
    displayMsgAndWait(Msg, 0);
    
}   // displayMsg()


void displayProgDetails() 
{
    String tmpString;

    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setColor(WHITE);
    display.drawString(5, 5, progDetails.ID);
    //display.drawRect(0, 0, 36, 36);
    display.drawRect(0, 0, 45, 36);
    display.setFont(ArialMT_Plain_16);
    if (progDetails.Lock) {
        tmpString = "[RO]";
        display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 0, tmpString);
    } else {
        tmpString = "[RW]";
        display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 0, "[RW]");
    }
    display.setFont(ArialMT_Plain_16);
    tmpString = "$" + String(progDetails.Strt);
    display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 20, tmpString);
    display.display();
    idxName = 0;
    
}   // displayProgDetails()


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