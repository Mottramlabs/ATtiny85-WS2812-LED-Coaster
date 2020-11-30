# ATtiny85 WS2812 LED Coaster
Date: 30th November 2020

---

A simple LED coaster using WS2812 addressable LED chips. Using the library [FastLED](http://fastled.io/). 

PCB's [Available from eBay](https://www.ebay.co.uk/sch/mlabs2018/m.html?_nkw=&_armrs=1&_ipg=&_from=)

![PCB Layout](https://github.com/Mottramlabs/ATtiny85-WS2812-LED-Coaster/blob/main/Board%20Details/PIX201109.jpg)

Pressing the push button, either SW1 or SW2 changes the mode, the options are.

<ol>Modes 0 - 5 are animations</ol>
<ol>Mode 6 - (single white LED) cycles through the above animations</ol>
<ol>Mode 7 - Orange</ol>
<ol>Mode 8 - Pink</ol>
<ol>Mode 9 - Cyan</ol>

The ATtiny85 can be programmed using the Arduino IDE using a USBasp USB programmer available on eBay. Add ATTinyCore to your boards manager and upload using the programmer not the normal serial upload. 

![](https://github.com/Mottramlabs/ATtiny85-WS2812-LED-Coaster/blob/main/Board%20Details/USBasp.jpg)

With a new ATtiny85 chip it is necessary to "burn the boot-loader" first for the FastLED to work correctly.