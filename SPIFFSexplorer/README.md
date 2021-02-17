# SPIFFSexplorer

If you have build the previous version of the "Solid State Tape Device for the (micro)KIM" you have no way to capture the files on the Tape Device to your computer or what ever.

If you now want to upgrade to Version 2.0 of this device you will loose all your precious programs .. but wait!

With the SPIFFSexplorer you cán capture these programs!

All you have to do is flash this code to your Tape Device, connect to your WiFi network and upload the files from the Tape Device to your PC.

But be careful! You have to make sure that the SPIFFS size you used when you flashed your device is the same as what you use to flash the SPIFFSexplorer!
If you fail to do so you might loose all your program files.

Be sure NOT to flash SPIFFS as that will definitaly erase your programs!!!

Connecting to your WiFi network is simple:

* At startup the SPIFFSexplorer will create a WiFi AccessPoint. Connect with your computer to this AP.
* Start your browser and go to "http://192.168.4.1"
* A page will present itself on which you can select your WiFi network and enter your WiFi password
* Click the [Save] button.

Now restart the SPIFFSexplorer and point your browser to:
<pre>
      http://SPIFFSexplorer.local/
</pre>
