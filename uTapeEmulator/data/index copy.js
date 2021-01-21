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
  let nameEdit = false;
    
  
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

    document.getElementById("displayMainPage").style.display       = "block";
  
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
      document.getElementById("buttonFFrwd").disabled = true;
    }
    else if ( payload.startsWith('freeButtons') )                  // not json!
    {
      console.log("parsePayload(Status): received [" + payload + "]");
      document.getElementById("buttonFRev").disabled  = false;
      document.getElementById("buttonRec").disabled   = false;
      document.getElementById("buttonPlay").disabled  = false;
      document.getElementById("buttonFFrwd").disabled = false;
      buttonsActive = true;
    
    } else if ( payload.startsWith('set') ) {                       // not json!
        console.log("parsePayload(Setting): received [" + payload + "]");
        singlePair   = payload.split(",");
        document.getElementById(singlePair[0].trim()).value = singlePair[1].trim();
    
    } else if ( payload.startsWith('Switch') ) {                    // not json!
        console.log("parsePayload(Switch): received [" + payload + "]");
        singlePair   = payload.split(",");
        document.getElementById(singlePair[0]).innerHTML = singlePair[1];
    
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
        document.getElementById('ID').innerHTML   = jsonMessage.ID;
        document.getElementById('Strt').innerHTML = "$"+jsonMessage.Strt+" &nbsp; ";
        document.getElementById('Lock').innerHTML = "Mode ["+jsonMessage.Lock+"] &nbsp; ";
        document.getElementById('Name').innerHTML = "&nbsp; &nbsp; "+jsonMessage.Name;
        document.getElementById('Name').value = jsonMessage.Name;
        document.getElementById('Name').readOnly = true;
        document.getElementById('buttonNameEdit').innerHTML = "&#9998";
        
      } else if (jsonMessage.msgType == "progDescription") 
      {
        console.log("parsed .. progDesc is ["+ JSON.stringify(jsonMessage)+"]");
        console.log("Discription Text ["+ jsonMessage.Text+"]");
        document.getElementById('Description').innerHTML   = jsonMessage.Text;
        
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
    if (!buttonsActive) return;

    if (buttonPressed == 'FREV') 
    {
      console.log("buttonPressed(FRev)");
      document.getElementById('Name').innerHTML = "wait .. |<< ..";
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
      document.getElementById('Name').innerHTML = "wait .. >>| ..";
      webSocketConn.send("Button: FFWRD");
    } else if (buttonPressed == 'nameEdit') 
    {
      console.log("buttonPressed(nameEdit)");
      if (nameEdit)
      {
        document.getElementById('buttonNameEdit').innerHTML = "&#9998;"; 
        document.getElementById('Name').readOnly = true;
        nameEdit = false;
      }
      else 
      {
        document.getElementById('buttonNameEdit').innerHTML = "&#128190;";  
        document.getElementById('Name').readOnly = false;
        nameEdit = true;
      }
      
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
