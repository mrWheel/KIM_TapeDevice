
# ATtinyPLL2

The KIM-1 cassette interface uses a PLL tuned to distinguish between the
high and low frequency tone. The output of this PLL is then fed into a
comparator to generate a logic '1' whenever the high frequency is detected,
and a logic '0' whenever the low frequency is detected. This logic signal
is analyzed by the KIM-1 to reconstruct the bit-stream stored on the cassette
tape. Each bit begins with a low-high transition, and the bit value can be
determine by the timing of the falling edge generated by the high-frequency
to low-frequency transition within each bit.

The job of the ATTiny841 is to perform the same function of the original PLL
and comparator: analyze the input signal, and generate a logic '1' output
whenever the high frequency is detected, and a logic '0' output whenever the
low frequency is detected.
It does this by starting a timer the moment the High freguency is detected
and stop the timer the moment the Low frequency is detected. If the duration
between the starting and ending is > 3.726 milliseconds, than register
a LOW. If it is < 3.726 millis seconds, than register a HIGH.

Bits are shifted into a byte and, if that byte represents a valid char
that char is send by the output-pin (UART TxD) to the TAPE_IN pin of the 
ESP8266 for further processing.

In fact: at first the bits are shifted into the byte and checked against
a SYNC char. If it is a SYNC char, from then on, it will build a new char
every 8 bits.

