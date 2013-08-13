### SparkFun MetaWatch Arduino Library

Use an Arduino/BlueSMiRF combo to control the bluetooth-enabled [MetaWatch]().

### How to use the library

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

### License

Beerware. Please use, reuse, and modify this code. If you find it useful, and we meet some day, buy me a beer.

-Jim Lindblom
<br>SparkFun Electronics
<br>August 13, 2013
