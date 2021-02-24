# Flash Firmware

There are two processors on the digital Tape Recorder. Both need to be flashed.
Be aware: you cannot power the digital Tape Recorder with an FTDI board or ICSP 
programmer (the 3v3 and 5v lines are not connected). You need to power the
digital Tape Recorder before you can flash the firmware!

## First flash the ESP8266
The setup for the Arduino IDE is in the main program file.
If you want to use the digital Tape Recorder with a genuine KIM-1 make shure you change
the define `_REAL_KIM1` from `false` to `true`!

You do need a FTDI board with a RESET and DTR pin. Many FTDI boards have DTR and CTS
on the header but you cannot flash the digital Tape Recorder with such a board. Most
FTDI board make use of a CH340 chipi (or something like that) that does have a RESET pin. It's easy to wire
this pin to the CTS pin on the outside (interrupt the CTS track before you solder the RESET line).

But luckely there are a lot of FTDI boards with the RESET pin already connected to the header.

Make sure you have a board that has 3v3 output or you will burn the ESP8266!!

Now connect the digital Tape Recorder to the FTDI board and plug the FTDI board in an USB port.

In the Arduino IDE select the right Serial port and upload the firmware.

Now you can proceed to the next step.

## Flash the ATtiny841
The setup for the Arduino IDE is also in the main program (ATtinyPLL2).

You need an ISP or ICSP programmer with a 0.2" connector.

Press and hold(!) the RESET (SW1) switch on the digital Tape Recorder and
flash the bootloader to the ATtiny841. 
Next upload the firmware to the ATtiny841.

Your digital Tape Recorder is now good to go!

