

static const char serverIndex[] PROGMEM =
  R"====(
  <!DOCTYPE html>
  <html charset="UTF-8">
  <style type='text/css'>
     body { background-color: lightblue;}
    .header h1 span { position: relative; top: 1px; left: 10px; }
    .nav-container a:hover { color: black; background: black; /* only for FSexplorer - Rest does not work */}
    .nav-clock {position: fixed; top: 1px; right: 10px; color: blue; font-size: small; text-align: right; padding-right: 10px; }
    .nav-left { flex: 2 1 0; text-align: left; width: 70% }
    .nav-right { flex: 1 1 0; text-align: right; width: 20%; }
    .nav-img { top: 1px; display: inline-block; width: 40px; height: 40px; }
    .nav-item { display: inline-block; font-size: 16px; padding: 10px 0; height: 20px; border: none; color: black; }
    .footer { position: fixed; left: 0; bottom: 0; width: 100%; color: gray; font-size: small; text-align: right; }
  </style>
  <body>
  <!-- <div class="header"> -->
    <div class="nav-container">
      <div class='nav-left'>
        <span class='nav-item'>
        <h1>
          <span id="sysName">SPIFFSexplorer</span> &nbsp; &nbsp; &nbsp;
          <span id="state" style='font-size: small;'>-</span> &nbsp;
        </h1>
        </span>
        <!--
        <a id='FSexplorer'  class='nav-img'>
                      <img src='/FSexplorer.png' alt='FSexplorer'>
        </a>
        -->
      </div>
    </div>  <!-- nav-container -->
<!--  </div>   header -->
  <hr>
  <div>
    <span id="redirect"></span>
  </div>  
  <div id=newElements>
  </div>
  <hr>
  <!-- REST -->

  <script>
  
  'use strict';

  let needReload = true;
  let singlePair;
  let singleFld;

  window.onload=bootsTrap;
  window.onfocus = function() {
    if (needReload) {
      window.location.reload(true);
    }
  };
  
  
  function bootsTrap() {
    needReload = true;
    
    //document.getElementById('FSexplorer').addEventListener('click',function() 
    //                                   { location.href = "/FSexplorer"; });
    window.location.replace("/FSexplorer");
   
  } // bootsTrap()
  
  </script>

  </body>
    <div class="footer">
      <hr>
      2021 &copy; Willem Aandewiel &nbsp; &nbsp;
    </div>
</html>

  )====";
