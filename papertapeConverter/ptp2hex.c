/*
***************************************************************************
**  Program  : ptp2hex
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License (see End of this file)
**
**  This program converts a PaperTape format file to a
**  program file (.hex) that can be used by the digitalTapeRecorder
**
**  Usage:
**          ptp2hex fileName.ptp
**
**  Output:
**          fileName.hex
**
**  Compile:    (only tested on MacOS)
**          cc ptp2hex.c -o ptp2hex
***************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
// #include <libc.h>
#include <stdbool.h>
#include <stdlib.h>

// 32 Matches the value of the MOS_TECH_BASIC_V1.1HEX I am debugging against.
// 48 matches the original behavior.
const int BYTES_PER_LINE = 32;

const char *FILE_EOL = "\r\n";


FILE *fIn;
FILE *fOut;

char ptpName[60];
char hexName[60];
char isHex[8];
char lineIn[200];
char leadIn[5];
char bytes[3];
char cIn;
char lowByte[3], highByte[3];
int  checkSum     = 0;
int  startAddr    = 0;
int  nextAddr     = 0;
int  bytesInLine  = 0;

//--------------------------------------------
//-- Remove LF and/or CR characters from the end of the string.
//--------------------------------------------
void strip_eol(char* s)
{
    /* Loop while the string ends with a CR or LF character. */
    while (strlen(s) > 0 &&
            (s[strlen(s) - 1] == '\n' ||
             s[strlen(s) - 1] == '\r'))
    {
        /* Erase the last character. */
        s[strlen(s) - 1] = '\0';
    }
}


//--------------------------------------------
//-- converts two bytes into an integer
//--------------------------------------------
int hex2int(char chh, char chl)
{
  char hl[3];
  hl[0] = chh;
  hl[1] = chl;
  hl[2] = 0;
  return (int)strtol(hl, NULL, 16);

} // hex2int()


//--------------------------------------------
//-- converts four bytes into an integer
//--------------------------------------------
int hex4int(char ch1, char ch2, char cl1, char cl2)
{
  char hl[5];
  hl[0] = ch1;
  hl[1] = ch2;
  hl[2] = cl1;
  hl[3] = cl2;
  hl[4] = 0;
  return (int)strtol(hl, NULL, 16);

} // hex4int()


//--------------------------------------------
//-- converts a char-string to uppercase
//--------------------------------------------
void to_upper(char* string)
{
    const char OFFSET = 'a' - 'A';
    while (*string)
    {
        *string = (*string >= 'a' && *string <= 'z') ? *string -= OFFSET : *string;
        string++;
    }
}


//----------------------------------------------------------
//-- read's file with the name inName
//-- and writes to a file outName
//----------------------------------------------------------
void processFile(char *inName, char *outName)
{
  int lineCount = 0, byteCount = 0, charCount = 0;
  int tmpSum = 0;
  checkSum   = 0; // globaly defined
  char  lc[10];

  memset(lineIn, 0, sizeof(lineIn));

  fIn  = fopen(inName, "r");
  if (!fIn)
  {
    printf("Error: file [%s] does not exist!\r\n", inName);
    return;
  }
  fOut = fopen(outName, "w");
  if (!fOut)
  {
    printf("Error: opening output file [%s]\r\n", outName);
    return;
  }

  //-- every line in the paperTape file has the following
  //-- format:
  //-- ;NNHHLLDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDCHCL
  //--
  //-- where:
  //-- NN = number of data bytes in the line
  //-- HH = Start High address of this line
  //-- LL = Start Low address of this line
  //-- D  = 1 nibble of data [0-9, A-F]
  //-- CH = checksum High Byte
  //-- CL = checksum Low Byte
  while(fgets(lineIn, sizeof(lineIn), fIn))
  {
    strip_eol(lineIn);

    lineCount++;
    //-- read HH and LL
    startAddr = hex4int(lineIn[3], lineIn[4], lineIn[5], lineIn[6]);
    if (lineCount == 1)
    {
      //-- HEX file starts with the programID (make it always "00")
      printf("00");
      fputs("00", fOut);

      //-- calculatie checkSum total
      tmpSum += hex2int(lineIn[5], lineIn[6]);
      snprintf(lc, sizeof(lc), "%c%c", lineIn[5], lineIn[6]);
      printf("%s", lc);
      //-- write program start address Low byte
      fputs(lc, fOut);
      tmpSum += hex2int(lineIn[3], lineIn[4]);
      snprintf(lc, sizeof(lc), "%c%c", lineIn[3], lineIn[4]);
      printf("%s", lc);
      //-- write program start address High byte
      fputs(lc, fOut);
      byteCount = -1;
      nextAddr = startAddr;
    }

    int eol = strlen(lineIn) -4;  //-- we are not interested in the last 4 chars
    //-- if lineIn length > 7 (";NNHHLL" = 7 positions)
    if (eol>7)
    {
      if (nextAddr < startAddr)
      {
        //-- if nextAddr < startAddr we have to add bytes into to hex file
        for (int f=nextAddr; f<startAddr; f++)
        {
          byteCount++;
          if (byteCount % BYTES_PER_LINE ==0)
          {
            printf("\n");
            fputs(FILE_EOL, fOut);
          }
          printf("00 ");
          //-- add NOP
          fputs("EA", fOut);
          tmpSum += hex2int('E', 'A');
          byteCount++;
        }
      }
      //-- write data from PTP to HEX
      for (int p=7; p<eol; p++)
      {
        byteCount++;
        charCount++;
        if (charCount==1)
        {
          bytes[0] = lineIn[p];   // put first nibble in bytes[0]
        }
        else
        {
          bytes[1]  = lineIn[p];  // place second nibble in bytes[1]
          bytes[2]  = 0;          // NULL to bytes[2]
          tmpSum += hex2int(bytes[0], bytes[1]);
          charCount = 0;
          printf("%s ", bytes);
          //-- write byte to file.hex
          fputs(bytes, fOut);
        }
        //-- time to write a newline (only for estetic reasons)?
        if (byteCount % BYTES_PER_LINE == 0)
        {
          printf("\n");
          fputs(FILE_EOL, fOut);
        }
      }
    }
    //-- read number of bytes in lineIn
    bytesInLine = hex2int(lineIn[1], lineIn[2]);
    //-- calculate nextAddr = startAddr + bytes in lineIn
    nextAddr  = startAddr+bytesInLine;
  }
  //-- this is not really necessary but it costs nothing extra
  for (int i=0; i<5;i++)
  {
    tmpSum += hex2int('E', 'A');
    fputs("EA", fOut);
  }
  printf("\r\nEOF\r\n");
  //-- write '/' (EOF marker) on a new line
  fputs(FILE_EOL, fOut);
  fputs("/", fOut);

  //-- calculate checkSum (High and Low)
  int checkSumH, checkSumL;

  checkSumL = (tmpSum&0x00FF);
  snprintf(lc, sizeof(lc), "%02x", checkSumL);
  to_upper(lc);
  //-- write checkSum Low Byte
  fputs(lc, fOut);
  checkSumH = (tmpSum&0xFF00) >> 8;
  snprintf(lc, sizeof(lc), "%02x", checkSumH);
  to_upper(lc);
  //-- write checkSum High Byte
  fputs(lc, fOut);
  printf("checkSum is [%04x] => [%02x][%02x]\r\n", tmpSum, checkSumL, checkSumH);

  //-- write a final newLine
  fputs(FILE_EOL, fOut);

  fclose(fOut);
  fclose(fIn);

} // processFile()

// 
//-- Strip an extension from a file name.
//-- Lifted from 
//-- https://stackoverflow.com/questions/43163677/how-do-i-strip-a-file-extension-from-a-string-in-c

void strip_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if ((end > fname && *end == '.') &&
        (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }  
}


//----------------------------------------------------------
int main(int argc, char* argv[])
{
  int arg;
  char *ext = ".hex";

  if (argv[1] == NULL)
  {
    printf("\r\nUse: ptp2hex <filein>.ptp\r\n");
    return 1;
  }
  snprintf(ptpName, sizeof(ptpName), "%s", argv[1]);
  printf("ptpName is [%s]\r\n", ptpName);
  if (strlen(ptpName) < 5)
  {
    printf("\r\nUse: ptp2hex <filein>.ptp\r\n");
    return 2;
  }

  snprintf(hexName, sizeof(hexName), "%s", ptpName);

  strip_ext(hexName);
  // Shorten the base file name to make room for the extension if needed.
  // No, the loop isn't really effencient but this covers a rare edge case and
  // makes the math easier.
  while (strlen(hexName) + strlen(ext) >= sizeof(hexName) +1 )
  {
    hexName[strlen(hexName) -1] = 0;
  }
  strcat(hexName, ext);
  
  printf("hexName is [%s]\r\n", hexName);

  processFile(ptpName, hexName);


} //  main()


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
