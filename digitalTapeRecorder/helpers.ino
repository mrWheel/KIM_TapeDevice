/*
***************************************************************************  
**  Program  : digitalTapeRecorder v2.0 / helpers
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License.                                                            
***************************************************************************  
*/

String words[20];


//===========================================================================================
int isHex(char nibbleIn)
{
  if (    (nibbleIn >= '0' && nibbleIn <= '9')
       || (nibbleIn >= 'A' && nibbleIn <= 'F') 
       || (nibbleIn >= 'a' && nibbleIn <= 'f') )
       return true;
       
  return false;
  
} //  isHex()


//===========================================================================================
int getPosition(const char *array, size_t arraySize, char value)
{
    int ret = -1;

    int i = 0;
    bool found = false;
    while (i < arraySize && !found)
    {
      found = (array[i++] == value);
      yield();
    }
    
    if (found)
          ret = i-1;
    else  ret = -1;

    return ret;
    
} // getPosition()


//===========================================================================================
void arrayToUpper(char *array, size_t arraySize)
{
  //for(int i=0; (i<arraySize && array[i] != 0); i++)
  for(int i=0; (array[i] != 0); i++)
  {
    if (array[i] >= 'a' && array[i] <= 'z')
    {
      array[i] = array[i] - ('a' - 'A');
    }
  }
  
} // arrayToUpper()


//===========================================================================================
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords) 
{
  int16_t inxS = 0, inxE = 0, wordCount = 0;
  
    inStrng.trim();
    while(inxE < inStrng.length() && wordCount < maxWords) 
    {
      yield();
      inxE  = inStrng.indexOf(delimiter, inxS);         //finds location of first ,
      wOut[wordCount] = inStrng.substring(inxS, inxE);  //captures first data String
      wOut[wordCount].trim();
      //DebugTf("[%d] => [%c] @[%d] found[%s]\r\n", wordCount, delimiter, inxE, wOut[wordCount].c_str());
      inxS = inxE;
      inxS++;
      wordCount++;
    }
    // zero rest of the words
    for(int i=wordCount; i< maxWords; i++)
    {
      wOut[wordCount][0] = 0;
    }
    // if not whole string processed place rest in last word
    if (inxS < inStrng.length()) 
    {
      wOut[maxWords-1] = inStrng.substring(inxS, inStrng.length());  // store rest of String      
    }

    return wordCount;
    
} // splitString()


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
