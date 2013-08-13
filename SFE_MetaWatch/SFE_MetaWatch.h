/* SFE_MetaWatch.h
	by: Jim Lindblom
	    SparkFun Electronics
	date: August 13, 2013
	license: Beerware. Please use, reuse, and modify this code as you see fit.
	If you find it useful, and we meet some day, buy me a beer.
	
	TODO: Add a function to read light sensor
	TODO: Add a function to draw a sprite
*/

#ifndef SFE_MetaWatch_H
#define SFE_MetaWatch_H

// The useful messages defined by the MetaWatch API
// http://www.metawatch.org/assets/images/developers/MetaWatchRemoteMessageProtocol205.pdf
#define MSG_RESET				0x07	// Reset watch
#define MSG_SET_VIBRATE_MODE	0x23	// vibrate
#define MSG_SET_RTC 			0x26	// set rtc (clock)
#define MSG_WRITE_LCD_BUFFER 	0x40	// draw on the LCD
#define MSG_CONTROL_FULL_SCREEN 0x42	// Swap between full and 2/3 screen control
#define MSG_UPDATE_LCD 			0x43	// Update LCD (actually performs the draw action)
#define MSG_LOAD_TEMPLATE 		0x44	// Clears the LCD (?)
#define MSG_DRAW_CLOCK			0x4E	// Draw clock
#define MSG_UPDATE_CLOCK		0x51	// Update clock display
#define MSG_SET_BACKLIGHT		0x5D	// Turn backlight on/off
#define MSG_GET_BATTERY 		0x56	// Get battery status
#define MSG_GET_LIGHT_SENSOR 	0x58	// Get light sensor value
#define MSG_IDLE_UPDATE			0xA0	// Update idle mode (?)
#define MSG_WIDGET_LIST 		0xA1	// Setup the widget list

// The are four modes the MetaWatch can be in:
#define MODE_IDLE 			0	// Standard 4-page idle mode (Swap pages by clicking B)
#define MODE_APP 			1	// App mode (Get here by clicking C)
#define MODE_NOTIFICATION 	2	// Notification mode (get here by clicking A)
#define MODE_MUSIC 			3	// Music mode (get here by clicking E)


// Widget Setting Bits. These defines should be used in conjuction
//	with the setWidget() function. These bits are already shifted!
#define NORMAL_WIDGET	0x00
#define CLOCK_WIDGET	0x80
#define NORMAL_COLOR	0x00
#define INVERT_COLOR	0x40
#define WIDGET_PAGE3	0x30
#define WIDGET_PAGE2	0x20
#define WIDGET_PAGE1	0x10
#define WIDGET_PAGE0	0x00
#define WIDGET_1Q		0x00	// One quad
#define WIDGET_2H		0x04	// 2 Horizontal
#define WIDGET_2V		0x08	// 2 Vertical
#define WIDGET_4Q		0x0C	// 4 (Full screen)
#define WIDGET_POS0		0x00
#define WIDGET_POS1		0x01
#define WIDGET_POS2		0x02
#define WIDGET_POS3		0x03

#define SUNDAY		0
#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRIDAY		5
#define	SATURDAY	6

// A delay between sending a command to the BlueSMiRF and receiving a response
// Very kludgy solution
#define BLUETOOTH_RESPONSE_DELAY 500 // TODO: Hone this value in, or better yet, find another way
#define CONNECT_TIMEOUT 10

// Varibales used for CRC calculation:
//	This code was found in this nifty python MetaWatch emulator:
//	https://github.com/leoluk/metawatch-simulator
//	
//	Which looks like it was in turn taken from a handy online
//	CRC calculator: http://zorc.breitbandkatze.de/crc.html.
const int order = 16;
const unsigned long polynom = 0x1021;//0x4c11db7;
const int direct = 1;
const unsigned long crcinit = 0xFFFF;
const unsigned long crcxor = 0x0;//0xffffffff;
const int refin = 1;
const int refout = 0;//1;

class SFE_MetaWatch
{
private:
	unsigned long reflect (unsigned long crc, int bitnum);
	unsigned long crcbitbybitfast(unsigned char* p, unsigned long len);
	int ComputeCRC(unsigned char* packet, int nBytes);
	
	unsigned long crcmask;
	unsigned long crchighbit;
	unsigned long crcinit_direct;
	unsigned long crcinit_nondirect;
	
	unsigned char connected;
	unsigned long baudRate;
	char watchAddress[12];
	
public:
	SFE_MetaWatch(char * addr, unsigned long baud);
	void begin();
	void echoMode();
	int connect();
	void setTime(unsigned int year, unsigned char month, unsigned char date, unsigned char weekDay, unsigned char hour, unsigned char minute, unsigned char second);
	void vibrate(unsigned int onTime, unsigned int offTime, unsigned char numCycles);
	void update(unsigned char page, unsigned char start=0, unsigned char end=96, unsigned char style=1, unsigned char buffer=1, unsigned char mode=0);
	void clear(unsigned char black);
	void setWidget(unsigned char msgTotal, unsigned char msgIndex, unsigned char * widgIDSet, unsigned char numWidg);
	void fullScreen(unsigned char full);
	void readBattery();
	void reset();
	void setBacklight(unsigned char set);
	void sendPacket(unsigned char * data, int length, unsigned char * response, int responseLength);
	
	//void drawClockWidget(unsigned char clockId);
	//void idleUpdate();
	//void updateClock();
	
	unsigned int batteryVoltage;
	unsigned char batteryCharge;
	unsigned char batteryCharging;
	unsigned char clipAttached;	
};

#endif	// SFE_MetaWatch_H