/*
***************************************************************************  
**  Program  : index.js, part of uTapeEmulator
**  Version  : v2.0.0   (10-01-2021)
**
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
  const APIGW='http://'+window.location.host+'/api/';

"use strict";

  let webSocketConn;
  let needReload  = true;
  let buttonsActive = true;
  let cPensil     = "&#9998;";
  let cDiskette   = "&#128190;";
  let cFRev       = "&#9194;";
  let cFFrwd      = "&#9193;";
  let cNextFree   = "&#9197;";
  let cRecord     = "&#9210;";
  let cPlay       = "&#9199;";
  let nameEdit    = false;
  let descEdit    = false;
  let lockEdit    = false;
  let gID         = 0;
  let gLock       = false;
    
  
  window.onload=bootsTrapMain;
  window.onfocus = function() {
    if (needReload) {
      window.location.reload(true);
    }
  };

      
  //============================================================================  
  function bootsTrapMain() 
  {
    console.log("bootsTrapMain()");
    needReload = true;

    let count = 0;
    while (document.getElementById('devVersion').value == "[version]") {
      count++;
      console.log("count ["+count+"] devVersion is ["+document.getElementById('devVersion').value+"]");
      if (count > 10) {
        alert("Count="+count+" => reload from server!");
        window.location.reload(true);
      }
      setTimeout("", 500);
    }

    document.getElementById('M_FSexplorer').addEventListener('click',function() 
                                                { console.log("newTab: goFSexplorer");
                                                  location.href = "/FSexplorer";
                                                });

    needReload = false;
    /*
    document.getElementById("displayMainPage").style.display       = "block";
    */
  
  } // bootsTrapMain()

    
  webSocketConn = new WebSocket('ws://'+location.host+':81/', ['arduino']);
  console.log(" ");
  console.log("WebSocket('ws://"+location.host+":81/', ['arduino'])");
  
  webSocketConn.onopen    = function () { 
    console.log("Connected!");
    webSocketConn.send('Connect ' + new Date()); 
    console.log("getDevInfo");
    webSocketConn.send("getDevInfo");
    needReload  = false;

  }; 
  webSocketConn.onclose     = function () { 
    console.log(" ");
    console.log("Disconnected!");
    console.log(" ");
    needReload  = true;
    let redirectButton = "<p></p><hr><p></p><p></p>"; 
    redirectButton    += "<style='font-size: 50px;'>Disconneted from uTape"; 
    redirectButton    += "<input type='submit' value='re-Connect' "; 
    redirectButton    += " onclick='window.location=\"/\";' />  ";     
    redirectButton    += "<p></p><p></p><hr><p></p>"; 

  }; 
  webSocketConn.onerror   = function (error)  { 
    console.log("Error: " + error);
    console.log('WebSocket Error ', error);
  };
  webSocketConn.onmessage = function (e) {
  //console.log("onmessage: " + e.data);
    parsePayload(e.data); 
  };
     
  
  //============================================================================  
  function parsePayload(payload) 
  {
    validJson = IsJsonString(payload);
    if ( payload.startsWith('blockButtons') )                       // not json!
    {
      console.log("parsePayload(Status): received [" + payload + "]");
      buttonsActive = false;
      document.getElementById("buttonFRev").disabled  = true;
      document.getElementById("buttonRec").disabled   = true;
      document.getElementById("buttonPlay").disabled  = true;
      document.getElementById("buttonEmpty").disabled = true;
      document.getElementById("buttonFFrwd").disabled = true;
    }
    else if ( payload.startsWith('freeButtons') )                  // not json!
    {
      console.log("parsePayload(Status): received [" + payload + "]");
      document.getElementById("buttonFRev").disabled  = false;
      document.getElementById("buttonRec").disabled   = false;
      document.getElementById("buttonPlay").disabled  = false;
      document.getElementById("buttonEmpty").disabled = false;
      document.getElementById("buttonFFrwd").disabled = false;
      document.getElementById('ID').style.display     = "block";
      document.getElementById('IDwait').style.display = "none";

      buttonsActive = true;
    
    } else if (validJson) 
    {
      jsonMessage = JSON.parse(payload);
      console.log("parsePayload(json): [" + jsonMessage.msgType + "]");
      if (jsonMessage.msgType == "devInfo")
      {
        document.getElementById("devVersion").innerHTML = jsonMessage.devVersion;
        console.log("parsePayload(): "+jsonMessage.devIPaddress);
        
      } else if (jsonMessage.msgType == "progDetails") 
      {
        console.log("parsed .. progDetails is ["+ JSON.stringify(jsonMessage)+"]");
        gID = Number("0x"+jsonMessage.ID);
        console.log("gID["+gID+"] -> Lock["+jsonMessage.Lock+"]");
        document.getElementById('ID').innerHTML   = jsonMessage.ID;
        document.getElementById('Strt').innerHTML = "Start [$"+jsonMessage.Strt+"]";
        console.log("jsonMessage.Lock is ["+jsonMessage.Lock+"]");
        if (jsonMessage.Lock == "1") 
        {
          gLock = true;
          console.log("(1) gLock to true");
        }
        else 
        {
          gLock = false;
          console.log("(1) gLock to false");
        }  
        if (gID >= 124) gLock = true;
        console.log("(2) gLock["+gLock+"]");
        document.getElementById('switchLock').checked = !gLock;
        document.getElementById('End').innerHTML = "&nbsp; End [$"+jsonMessage.End+"] ";
        document.getElementById('Name').innerHTML = "&nbsp; &nbsp; "+jsonMessage.Name;
        document.getElementById('Name').value = jsonMessage.Name;
        document.getElementById('Name').readOnly = true;
        document.getElementById('buttonNameEdit').innerHTML = cPensil;
        document.getElementById('buttonDescEdit').innerHTML = cPensil;
        document.getElementById('ID').style.display = "block";
        document.getElementById('IDwait').style.display = "none";
        
      } else if (jsonMessage.msgType == "progDescription") 
      {
        console.log("parsed .. progDesc is ["+ JSON.stringify(jsonMessage).substring(0, 20)+"]");
        //console.log("Discription Text ["+ jsonMessage.Text+"]");
        let newText = jsonMessage.Text.replaceAll("@n$", "\n");
        newText = newText.replaceAll("@1$", ":");
        console.log("Discription Text ["+ newText.substring(0,20)+"]");

        //document.getElementById('Description').innerHTML   = jsonMessage.Text;
        document.getElementById('Description').innerHTML   = newText;
        document.getElementById('Description').value       = newText;
        
      } else if (jsonMessage.msgType == "sendHistInTemp") 
      {
        let obj = JSON.parse(payload);
        for ( var i = 0; i < obj.inTemp1.length; i++ ) {
          tempIn.push( obj.inTemp1[i] );
        };  // for ..
      } else {
      console.log("parsePayload(): Don't know: [" + payload + "]\r\n");
      }
    } // isValisJson()
  };  // payload()
 
  
  //============================================================================  
  function handleButton(buttonPressed) {
    console.log("handleButton("+buttonPressed+")");

    if (!buttonsActive) return;
      
    if (buttonPressed == 'lockEdit') 
    {
      console.log("buttonPressed(lockEdit)");
      if (lockEdit)
      {
        if (gID == 0 || gID >= 224) 
        {
          gLock = true;
          document.getElementById('switchLock').checked = false;
        }
        console.log("newLock:" + gLock);
        webSocketConn.send("newLock:" + gLock);
        document.getElementById('buttonLockEdit').innerHTML = cPensil; 
        lockEdit = false;
      }
      else 
      {
        document.getElementById('switchLock').checked = !gLock;
        document.getElementById('buttonLockEdit').innerHTML = cDiskette;  
        lockEdit = true;
      }
    } // lockEdit
    
    if (   buttonPressed == "FREV" || buttonPressed == "REC" 
        || buttonPressed == "PLAY" || buttonPressed == "FFWRD"
        || buttonPressed == "EMPTY" )
    {
      document.getElementById('ID').style.display = "none";
      document.getElementById('IDwait').style.display = "block";
      document.getElementById('buttonLockEdit').innerHTML = cPensil; 
      document.getElementById('switchLock').checked = !gLock;
      lockEdit = false;
    }
    
    if (buttonPressed == 'FREV') 
    {
      console.log("buttonPressed(FRev)");
      document.getElementById('Name').value = "Rewinding tape ...";
      document.getElementById('Description').value = "";
      webSocketConn.send("Button: FREV");

    } else if (buttonPressed == 'REC') 
    {
      console.log("buttonPressed(REC)");
      document.getElementById('Name').innerHTML = "wait while Recording ...";
      webSocketConn.send("Button: REC");

    } else if  (buttonPressed == 'PLAY') 
    {
      console.log("buttonPressed(PLAY)");
      document.getElementById('Name').innerHTML += ".. Playing ...";
      webSocketConn.send("Button: PLAY");

    } else if (buttonPressed == 'FFWRD') 
    {
      console.log("buttonPressed(FFWRD)");
      document.getElementById('Name').value = "Fast forward tape ...";
      document.getElementById('Description').value = "";
      webSocketConn.send("Button: FFWRD");

    } else if (buttonPressed == 'EMPTY') 
    {
      console.log("buttonPressed(FFWRD)");
      document.getElementById('Name').value = "Fast forward tape ...";
      document.getElementById('Description').value = "";
      webSocketConn.send("Button: EMPTY");
      
    } else if (buttonPressed == 'nameEdit') 
    {
      console.log("buttonPressed(nameEdit)");
      if (gID == 0 || gID >= 224 || gLock || lockEdit) return;
      if (nameEdit)
      {
        console.log("newName:" + document.getElementById('Name').value);
        webSocketConn.send("newName:" + document.getElementById('Name').value);
        //--- set to readonly ----
        document.getElementById('buttonNameEdit').innerHTML = cPensil; 
        document.getElementById('Name').readOnly = true;
        nameEdit = false;
      }
      else 
      {
        //--- open for edit ----
        document.getElementById('buttonNameEdit').innerHTML = cDiskette;  
        document.getElementById('Name').readOnly = false;
        nameEdit = true;

      }
      
    } else if (buttonPressed == 'descEdit') 
    {
      console.log("buttonPressed(descEdit)");
      if (gID == 0 || gID >= 224 || gLock || lockEdit) return;
      if (descEdit)
      {
        //console.log("newName:" + document.getElementById('Name').value);
        webSocketConn.send("newDesc:" + document.getElementById('Description').value);
        //--- set to readonly ----
        document.getElementById('buttonDescEdit').innerHTML = cPensil; 
        document.getElementById('Description').readOnly = true;
        document.getElementById('Description').style.backgroundColor = "lightblue";
        descEdit = false;
      }
      else 
      {
        //--- open for edit ----
        document.getElementById('buttonDescEdit').innerHTML = cDiskette;  
        document.getElementById('Description').readOnly = false;
        document.getElementById('Description').style.backgroundColor = "lightgray";
        descEdit = true;
      }

    } else if (buttonPressed == 'setLock') 
    {
      console.log("buttonPressed(setLock)");
      console.log("==>switchLock["+document.getElementById('switchLock').checked+"]");
      if (!lockEdit)
      {
        document.getElementById('switchLock').checked = !gLock;
        return;
      }      
      //--- the radio button is false if ReadOnly!
      //--- gLock true/1  => ReadOnly
      //--- gLock false/0 => Read/Write
      if (document.getElementById('switchLock').checked)
            gLock = false;
      else  gLock = true;
      console.log("==>gLock["+gLock+"]");
      //--- set to readonly ----
      document.getElementById('buttonDescEdit').innerHTML = cPensil; 
      document.getElementById('Description').readOnly = true;
      nameEdit = false;
      descEdit = false;
      
    }

  } // handleButton()

  
   
  //============================================================================  
  function setBackGround(field, newColor) {
    //console.log("setBackground("+field+", "+newColor+")");
    document.getElementById(field).style.background = newColor;
    
  } // setBackGround()

   
  //============================================================================  
  function getBackGround(field) {
    //console.log("getBackground("+field+")");
    return document.getElementById(field).style.background;
    
  } // getBackGround()

  
  //============================================================================  
  function round(value, precision) {
    var multiplier = Math.pow(10, precision || 0);
    return Math.round(value * multiplier) / multiplier;
  }

  //============================================================================  
  function IsJsonString(str) {
    try {
        JSON.parse(str);
    } catch (e) {
        return false;
    }
    return true;
  } // IsJsonString()

  
  //============================================================================  
  function printAllVals(obj) {
    for (let k in obj) {
      if (typeof obj[k] === "object") {
        printAllVals(obj[k])
      } else {
        // base case, stop recurring
        console.log(obj[k]);
      }
    }
  } // printAllVals()
  
  
  //============================================================================  
  String.prototype.replaceAll = function(str1, str2, ignore) 
  {
    return this.replace(new RegExp(str1.replace(/([\/\,\!\\\^\$\{\}\[\]\(\)\.\*\+\?\|\<\>\-\&])/g,"\\$&"),(ignore?"gi":"g")),(typeof(str2)=="string")?str2.replace(/\$/g,"$$$$"):str2);
  //return this.replace(new RegExp(str1.replace(/([\/\,\!\\\^\$\[\]\(\)\.\*\+\?\|\<\>\-\&])/g,"\\$&"),(ignore?"gi":"g")),(typeof(str2)=="string")?str2.replace(/\$/g,"$$$$"):str2);
  }
   
/*
***************************************************************************
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
***************************************************************************
*/
