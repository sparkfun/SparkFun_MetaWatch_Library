SparkFun MetaWatch Arduino Library
===================================


Use an Arduino/[BlueSMiRF](https://www.sparkfun.com/products/12577) combo to control the bluetooth-enabled [MetaWatch](https://www.sparkfun.com/products/12005).

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager. 


Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.
* **[Hookup Guide](https://learn.sparkfun.com/tutorials/metawatch-teardown-and-arduino-hookup)** - Basic hookup guide for the MetaWatch.


License Information
-------------------

The **code** is beerware; if you see me (or any other SparkFun employee) at the local, and you've found our code helpful, please buy us a round!

Please use, reuse, and modify these files as you see fit. Please maintain attribution to SparkFun Electronics and release anything derivative under the same license.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.


How to use the library
--------------------------
This library provides the following functions:

* SFE_MetaWatch constructor
* void begin()
* void echoMode();
* int connect();
* void setTime(unsigned int year, unsigned char month, unsigned char date, unsigned char weekDay, unsigned char hour, unsigned char minute, unsigned char second);
* void vibrate(unsigned int onTime, unsigned int offTime, unsigned char numCycles);
* void update(unsigned char page, unsigned char start=0, unsigned char end=96, unsigned char style=1, unsigned char buffer=1, unsigned char mode=0);
* void clear(unsigned char black);
* void setWidget(unsigned char msgTotal, unsigned char msgIndex, unsigned char * widgIDSet, unsigned char numWidg);
* void fullScreen(unsigned char full);
* void readBattery();
* void reset();
* void setBacklight(unsigned char set);
* void sendPacket(unsigned char * data, int length, unsigned char * response, int responseLength);

Author
--------

Jim Lindblom
<br>SparkFun Electronics
<br>August 13, 2013
