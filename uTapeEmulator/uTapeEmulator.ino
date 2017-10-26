/*
***************************************************************************  
**  Program  : uTapeEmulator v3
**  Copyright (c) 2017 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************  
*/

#include <ESP.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SSD1306.h>

#define _BV(bit)            (1 << (bit))
#define _SET(a,b)           ((a) |= _BV(b))
#define _CLEAR(a,b)         ((a) &= ~_BV(b))

#define dBegin(...)     {if (doVerbose) { Serial.begin(__VA_ARGS__); } }
#define dPrint(...)     {if (doVerbose) { Serial.print(__VA_ARGS__); } }
#define dPrintln(...)   {if (doVerbose) { Serial.println(__VA_ARGS__); } }
#define dPrintf(...)    {if (doVerbose) { Serial.printf(__VA_ARGS__); } }
#define dFlush(...)     {if (doVerbose) { Serial.flush(__VA_ARGS__); } }


#define _LED_PIN        BUILTIN_LED

#define _SDA            4       // D2   - GPIO04
#define _SCL            5       // D1   - GPIO05

#define _TAPE_IN_PIN    12      // D6   - GPIO12
#define _TAPE_OUT_PIN   13      // D7   - GPIO13

#define _PLAY_BUTTON    0       // A0
#define _FREV_BUTTON    14      // D5
#define _FFWD_BUTTON    16      // D0

#define _DATAROOT        "/KIM"
#define _DIMOLEDTIME    300000  // five minutes


// Initialize the OLED display using Wire library
SSD1306  display(0x3c, _SDA, _SCL);

extern void dspl_Init();

struct { uint8_t  Num; 
         char     ID[3];
         char     Strt[5];
         char     Desc[50];
         bool     Lock; 
       } progDetails;

char            ascii[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

int16_t         progCounter;

uint32_t        getTimingDuration, dimOledTimer;
uint32_t        bitStartTime, bitTime;
byte            actByte;
int8_t          fileNum = 0;
char            fileID[5];
char            filePath[50];
char            cBuf[50], scrollDesc[50];
char            lByte, hByte;
int16_t         tmpB, idxDesc;
uint32_t        scrollInterval;
char            buttonChoice, mChoice;
bool            doVerbose = false;
bool            displayIsOff;

// Prints out Binary value (1 or 0) of byte
void printByte(int c) {
    if (!doVerbose) return;
    for (int bit = 7; bit >= 0; bit--) {
        // Compare bit 7-0 in byte
        if (c & (1 << bit)) {
            Serial.print ("1");
        } else {
            Serial.print ("0");
        }
    }
}   // printByte()


bool buttonPressed() {
    int16_t aVal;

    aVal = analogRead(_PLAY_BUTTON);
    if (aVal > 800) {
        delay(100);
        aVal = analogRead(_PLAY_BUTTON);
        if (aVal > 800) {
            dPrintf("Record button! [%d]\n", aVal);
            buttonChoice = 'R';
            while (analogRead(_PLAY_BUTTON) > 800) { yield(); }
            return true;
            
        } else return false;
    } else {
        if (aVal > 300 && aVal < 800) {
            delay(100);
            aVal = analogRead(_PLAY_BUTTON);
            if (aVal > 300 && aVal < 800) {
                dPrintf("Play button! [%d]\n", aVal);
                buttonChoice = 'P';
                while (analogRead(_PLAY_BUTTON) > 300) { yield(); }
                return true;
                
            } else return false;
        } 
    }
    if (digitalRead(_FFWD_BUTTON) == LOW) {
        delay(100);
        if (digitalRead(_FFWD_BUTTON) == LOW) {
            dPrintln("FastForward button!");
            buttonChoice = '+';
            while (digitalRead(_FFWD_BUTTON) == LOW) { yield(); }
            return true;
        }
    }
    if (digitalRead(_FREV_BUTTON) == LOW) {
        delay(100);
        if (digitalRead(_FREV_BUTTON) == LOW) {
            dPrintln("FastReverse button!");
            buttonChoice = '-';
            while (digitalRead(_FREV_BUTTON) == LOW) { yield(); }
            return true;
        }
    }

    return false;

}   // buttonPressed()

//------------------------------------------------------
// list files in datadir
//------------------------------------------------------
void lsFiles(bool fromRoot) {

    uint8_t fileNr = 0;
    uint8_t lenName;
    String space = "                           ";
    Dir dir;
    dir = SPIFFS.openDir("");

    Serial.printf("List files on Tape\n");
    while (dir.next()) {
        fileNr++;
        lenName = (25 - dir.fileName().length());
        //Serial.print(dir.fileName());
        File f = dir.openFile("r");
        //Serial.print(space.substring(0, (25 - dir.fileName().length())));
        if (fromRoot) {
            Serial.printf("[--] %s %s %d\n" , dir.fileName().c_str()
                                            , space.substring(0, lenName).c_str()
                                            , f.size());
        } else {
            Serial.printf("[%2d] %s %s %d\n", fileNr, dir.fileName().c_str()
                                            , space.substring(0, lenName).c_str()
                                            , f.size());
            
        }
    }
    Serial.printf("%d files in [/]\n", fileNr);

}   // lsFiles()


bool eraseFile() {

    Serial.printf("Initializing [%s]\n", filePath);
    if (SPIFFS.exists(filePath)) {
        if (!SPIFFS.remove(filePath) ) {
            Serial.printf("Error initializing file %s\n", fileID);
            return false;
        }
    }

    return true;

}   // eraseFile()


void tockelLock(uint8_t fileNum) {
    char cIn;
    
    setProgDetails(fileNum);

    sprintf(filePath, "%s/lock/%s", _DATAROOT, progDetails.ID);
    if (progDetails.Lock) {
        SPIFFS.remove(filePath);
    } else {
        File fileLock = SPIFFS.open(filePath, "a");
        fileLock.close();
    }

    setProgDetails(fileNum);

}   // tockelLock()


void setDescription(uint8_t fileNum) {
    char cIn;

    if (fileNum == 0) {
        Serial.println("\nError: cannot alter description of Program 00\n");
    }
    setProgDetails(fileNum);
    if (progDetails.Lock) {
        Serial.println("\nError: program is locked!\n");
        return;
    }
    Serial.printf("\nEnter description for Program [%s] ? ", progDetails.ID);
    while (Serial.available()) {
        cIn = Serial.read();    // empty buffer
    }
    tmpB = 0;
    cIn  = 0;
    while (cIn != '\r' && cIn != '\n' && tmpB < 40) {
        yield();
        if (Serial.available()) {
            cIn = Serial.read();
            Serial.print(cIn);
            if (cIn >= ' ' && cIn <= '}') {
                progDetails.Desc[tmpB++] = cIn;
            }
        }
    }
    progDetails.Desc[tmpB] = '\0';

    sprintf(filePath, "%s/desc/%s", _DATAROOT, progDetails.ID);
    if (tmpB > 1) {
        File descFile = SPIFFS.open(filePath ,"w");
        for (tmpB = 0; progDetails.Desc[tmpB] != 0; tmpB++) {
            descFile.write(progDetails.Desc[tmpB]);
        }
        descFile.write('\0');
        descFile.close();
    } else {
        SPIFFS.remove(filePath);
    }
    Serial.println();
    
}   // setDescription()


void setProgDetails(uint8_t fileNum) {
    progDetails.Num = fileNum;
    if (fileNum < 0x10) 
            sprintf(progDetails.ID, "0%s",  String(fileNum, HEX).c_str());
    else    sprintf(progDetails.ID, "%s", String(fileNum, HEX).c_str());
    //sprintf(progDetails.ID, "%s", String(progDetails.ID.toUpperCase()));
    progDetails.ID[0] = toupper(progDetails.ID[0]);
    progDetails.ID[1] = toupper(progDetails.ID[1]);

        // -- get Start address ---
    sprintf(filePath, "%s/prgs/%s.hex", _DATAROOT, progDetails.ID);
    if (SPIFFS.exists(filePath)) {
        File fileData = SPIFFS.open(filePath, "r");
        fileData.read();    // skip ID lNible
        fileData.read();    // skip ID hNible
        progDetails.Strt[2] = (char)fileData.read();
        progDetails.Strt[3] = (char)fileData.read();
        progDetails.Strt[0] = (char)fileData.read();
        progDetails.Strt[1] = (char)fileData.read();
        progDetails.Strt[4] = 0;
        fileData.close();
    } else {
        sprintf(progDetails.Desc, "<Empty Slot>");
        progDetails.Strt[0] = '?';
        progDetails.Strt[1] = '?';
        progDetails.Strt[2] = '?';
        progDetails.Strt[3] = '?';
        progDetails.Strt[4] = '\0';
    }

    sprintf(filePath, "%s/desc/%s", _DATAROOT, progDetails.ID);
    if (SPIFFS.exists(filePath)) {
        File fileData = SPIFFS.open(filePath, "r");
        tmpB = 0;
        while (fileData.available()) {
            progDetails.Desc[tmpB++] = fileData.read();
        }
        fileData.close();
        progDetails.Desc[tmpB++] = '\0';
    } else {
        if (progDetails.Strt[0] == '?') {
            sprintf(progDetails.Desc, "<Empty Slot>");
            
        } else {
            sprintf(progDetails.Desc, "Un-named Program");
            sprintf(filePath, "%s/desc/%s", _DATAROOT, progDetails.ID);
            File descFile = SPIFFS.open(filePath ,"w");
            for (tmpB = 0; progDetails.Desc[tmpB] != 0; tmpB++) {
                descFile.write(progDetails.Desc[tmpB]);
            }
            descFile.write('\0');
            descFile.close();
        }
    }
    sprintf(filePath, "%s/lock/%s", _DATAROOT, progDetails.ID);
    if (SPIFFS.exists(filePath)) {
        progDetails.Lock  = true;
    } else {
        progDetails.Lock  = false;
    }
    if (fileNum == 0) { // this is a special kindof file!
        sprintf(progDetails.Desc, "ScratchPad");
        progDetails.Lock    = false;
    }

}   // setProgDetails()


void turnOffOledDisplay() {
    display.clear();
    display.display();
    displayIsOff = true;
    
}   // turnOffOledDisplay()


void turnOnOledDisplay() {
    
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

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
    dimOledTimer = millis() + _DIMOLEDTIME;

}   // turnOnOledDisplay()


void displayErrorMsg(String Msg, uint8_t wait) {
    yield();
    sprintf(scrollDesc, Msg.c_str());
    if (wait)   display.setColor(WHITE);
    else        display.setColor(BLACK);
    display.fillRect(0, 45, DISPLAY_WIDTH, (DISPLAY_HEIGHT - 45));
    display.setFont(ArialMT_Plain_16);
    if (wait)   display.setColor(BLACK);
    else        display.setColor(WHITE);
    display.drawString(0, 45, Msg);
    display.display();
    if (wait) {
        delay(wait * 1000);
    }
    
}   // displayErrorMsg()


void displayMsg(String Msg) {
    displayErrorMsg(Msg, 0);
    
}   // displayMsg()


void displayProgDetails() {
    String tmpString;
    //display.setColor(BLACK);
                //   x,  y, width,  height);
    //display.fillRect(0, 0, 128, 29);
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setColor(WHITE);
    display.drawString(5, 5, progDetails.ID);
    display.drawRect(0, 0, 36, 36);
    display.setFont(ArialMT_Plain_16);
    if (progDetails.Lock) {
        tmpString = "[RO]";
        display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 0, tmpString);
    } else {
        tmpString = "[R/W]";
        display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 0, "[R/W]");
    }
    display.setFont(ArialMT_Plain_16);
    tmpString = "$" + String(progDetails.Strt);
    display.drawString((DISPLAY_WIDTH - display.getStringWidth(tmpString)), 20, tmpString);
    display.display();
    idxDesc = 0;
    
}   // displayProgDetails()


void scrollProgDesc() {
    int16_t pBuff;

    String sBuff = String(progDetails.Desc).c_str();
    if (sBuff.length() < 16) {
        display.setColor(WHITE);
        display.drawString(0, 45, String(sBuff));
        display.display();
        return;
    }
    
    display.setColor(BLACK);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 45, String(scrollDesc));
    sBuff.concat("       ");
    if (idxDesc > sBuff.length()) {
        idxDesc = 0;
        return;
    }
    pBuff = idxDesc++;
    for (int p=0; p<16; p++) {
        yield();
        if (sBuff[pBuff] == 0) {
            pBuff   = 0;
            scrollDesc[p] = ' ';
        } else {
            scrollDesc[p] = sBuff[pBuff++];
        }
    }
    scrollDesc[16] = 0;
    display.setColor(WHITE);
    display.drawString(0, 45, String(scrollDesc));
    display.display();
    
}   // scrollProgDesc()


void printMenu() {
    int8_t sp, p;
    for (p=0; progDetails.Desc[p] != 0; p++) {};
    
    Serial.print("\nMenu");
    for (sp=p; sp < 38; sp++) Serial.print(" ");
    Serial.printf("[%s]", progDetails.ID); 
    if (progDetails.Lock)   Serial.print("* ");
    else                    Serial.print("  ");
    Serial.printf("- %s\n", progDetails.Desc);
    Serial.println("--------------------------------------------------");
    Serial.println("   P    - Play from Tape (uKIM enter [0x1800 G])");
    Serial.println("   R    - Record to Tape (uKIM enter [0x1873 G])");
    Serial.println("   +    - FForw. to next file on tape ");
    Serial.println("   -    - FRev. to previous file on tape ");
    Serial.print("   V    - Verbose  ");
    if (doVerbose) Serial.println("OFF");
    else           Serial.println("ON");
    Serial.printf( "   D    - Enter description for Program [%s]\n", progDetails.ID);
    Serial.printf( "   T    - Tockel Lock for Program [%s]\n", progDetails.ID);
    Serial.println("   L    - List files on tape ");
    Serial.println("   H    - This menu ");
    Serial.println(); 
    Serial.print(" (P/R/L/D/T/H) ? ");

    displayProgDetails();
    
}   // printMenu()


void setup() {
    WiFi.mode(WIFI_OFF);
    
    Serial.begin(19200);
    Serial.println();
    Serial.println("\n[uTapeEmulator (v2)]");
    Serial.flush();

    turnOnOledDisplay();
    delay(1000);

    pinMode(_LED_PIN, OUTPUT);
    digitalWrite(_LED_PIN, HIGH);
    pinMode(_TAPE_IN_PIN, INPUT);
    pinMode(_TAPE_OUT_PIN, OUTPUT);
    pinMode(_PLAY_BUTTON, INPUT);
    pinMode(_FFWD_BUTTON, INPUT_PULLUP);
    pinMode(_FREV_BUTTON, INPUT_PULLUP);

    SPIFFS.begin();
    SPIFFS.remove("/KIM/desc/0");
    SPIFFS.remove("/KIM/prgs/1.hex");
    SPIFFS.remove("/KIM/desc/1");
    SPIFFS.remove("/KIM/lock/1");
    
    Serial.println();

    setProgDetails(0);
    
    printMenu();

}   // setup()


void loop() {
    char cIn = 0;

    while (cIn != '\r' && cIn != '\n' && !buttonPressed()) {
        yield();
        if (millis() > dimOledTimer) {
            turnOffOledDisplay();
        
        } else if (millis() > scrollInterval) {
            if (idxDesc == 0) scrollInterval = millis() + 3000;
            else              scrollInterval = millis() + 250;
            scrollProgDesc();
        }
        if (Serial.available()) {
            cIn = Serial.read();
            if (cIn > ' ' && cIn <= 'z') {
                mChoice = cIn;
            }
        }
    }
    if (buttonChoice != 0) {
        mChoice = buttonChoice;
        buttonChoice = 0;
    }
    if (mChoice != 0) {
        if (displayIsOff) {
            switch (mChoice) {
                case '+':
                case '-':   turnOnOledDisplay();
                            printMenu();
                            mChoice = 0;
            }
            
        } else {

            Serial.printf(" choise is [%c]\n", mChoice);
            switch (mChoice) {
                case 'd':
                case 'D':   setDescription(fileNum);
                            setProgDetails(fileNum);
                            printMenu();
                            break;
                case 'l':
                case 'L':   lsFiles(true);
                            printMenu();
                            break;
                case 'p':
                case 'P':   playbackTape();
                            printMenu();
                            break;
                case 'r':
                case 'R':   recordTape();
                            setProgDetails(fileNum);
                            printMenu();
                            break;
                case 't':
                case 'T':   tockelLock(fileNum);
                            printMenu();
                            break;
                case 'v':
                case 'V':   doVerbose = !doVerbose;
                            printMenu();
                            break;
                case '+':   sprintf(filePath, "%s/prgs/%s.hex", _DATAROOT, progDetails.ID);
                            if (SPIFFS.exists(filePath)) {
                                fileNum++;
                            }
                            if (fileNum > 0xFF) fileNum = 0xFF;
                            setProgDetails(fileNum);
                            scrollInterval = millis();
                            printMenu();
                            break;
                case '-':   fileNum--;
                            if (fileNum < 0) fileNum = 0;
                            setProgDetails(fileNum);
                            scrollInterval = millis();
                            printMenu();
                            break;
                default:    printMenu();
            }
            mChoice = 0;
            dimOledTimer = millis() + _DIMOLEDTIME;
        }
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
***************************************************************************/
