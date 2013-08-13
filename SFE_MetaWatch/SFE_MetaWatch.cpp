/* SFE_MetaWatch.cpp
	by: Jim Lindblom
	    SparkFun Electronics
	date: August 13, 2013
	license: Beerware. Please use, reuse, and modify this code as you see fit.
	If you find it useful, and we meet some day, buy me a beer.
	
	This file has most of the code used to interface from Arduino to BlueSMiRF to MetaWatch.
*/

#include "Arduino.h"
#include "SFE_MetaWatch.h"
#include <SoftwareSerial.h>

// Initialize Software Serial
// TODO: Probably should make these pins definable from the sketch
SoftwareSerial bt(10, 11); // 10=RX, 11=TX

/* SFE_MetaWatch constructor
	Assign the BlueSMiRF baud (baudRate) and bluetooth address (addr) variables. 
	- baud should be the baud rate that the BlueSMiRF is set to
	- addr should be a 12-byte HEX address matching your MetaWatch's BT address
		e.g.: "0018342F9B56"
*/
SFE_MetaWatch::SFE_MetaWatch(char * addr, unsigned long baud)
{
	baudRate = baud;
	for (int i=0; i<12; i++)
	{
		watchAddress[i] = addr[i];
	}
}

/* begin() simply calls bt.begin() at the specified baud rate.
	Analagous to Serial.begin(baudRate)
*/
void SFE_MetaWatch::begin()
{
	bt.begin(baudRate);
}

/* connect() does its very best to initialize a connection between BlueSMiRF and MetaWatch.
	It'll have to enter command mode (or it might already be there).
	Then it enters the Connect command (C,address<newline>) (or it may already be connected)
	Then it checks for either a "TRYING" or "ERR-connected" response from the SMiRF.
	
	returns:
		2 if already connected
		1 if connection at least TRYING
		-2 if failure entering command mode (more common, usually baud mismatch)
		-1 if failure on connect command
*/
// TODO: This is extremely kludgy right now. 
//		The delay()'s should be made more robust, faster by periodically checking for a response,
//		while still checking for a timeout.
//		Checking for just the first char of a response is a questionabl solution.
//		Needs more testing.  So many unknown states the BlueSMiRF could be in. 
//		We need some flow chart action here.
int SFE_MetaWatch::connect()
{
	char c;
	int timeout = CONNECT_TIMEOUT;
	
	bt.print('\r');	// Clear any previous commands
	delay(BLUETOOTH_RESPONSE_DELAY);
	bt.flush();
	// Entering command mode. Should either print "CMD" or "?" if already there
	while ((c != 'C') && (c != '?') && (timeout > 0))
	{
		bt.print("$$$");	// Enter command mode
		delay(BLUETOOTH_RESPONSE_DELAY);
		c = bt.read();	// Read first character of response
		bt.flush();
		timeout--;
	}
	
	if (timeout == 0)
		return -2;
	else
		timeout = CONNECT_TIMEOUT;
	
	// After sending connect command, should print "TRYING", may also print "ERR-connected"
	while ((c != 'T') && (c != 'E') && (timeout > 0)) 
	{
		bt.print("C,");
		bt.print(watchAddress);
		bt.print('\r');
		delay(BLUETOOTH_RESPONSE_DELAY);
		c = bt.read();
		bt.flush();
		timeout--;
	}
	// If there was an error, try to exit command mode
	if ((c == 'E') || timeout == 0)
	{
		bt.print("---");	// Exit command mode
		bt.print('\r');
		delay(BLUETOOTH_RESPONSE_DELAY);
		bt.flush();
	}
	
	if (timeout == 0)
		return -1;	// Return -1 if connect command error
	else if (c == 'E')
		return 2;	// Return 2 if we think we're already connected
	else
		return 1;	// Return 1 if all went according to plan
}

/* setTime() sends the SET_RTC message to the MetaWatch
	year should be the actual integer year (e.g. 2013)
	month and date are the numbered dates (e.g. 8 and 13 respectively for August 13th)
	weekDay is 0-6 for Sunday through monday
	hour is of the 24-hour variety (e.g. 13 = 1pm)
	minute and second should be between 0 and 59
*/
void SFE_MetaWatch::setTime(unsigned int year, unsigned char month, unsigned char date, 
 unsigned char weekDay, unsigned char hour, unsigned char minute, unsigned char second)
{
	int nPacketBytes = 14;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_SET_RTC; // Set real time clock message
	packet[3] = 0; // options

	packet[4] = (year & 0xFF00) >> 8; // Year MSB
	packet[5] = (year & 0xFF);  // Year LSB
	packet[6] = month; // Month
	packet[7] = date; // Date of month
	packet[8] = weekDay; // Day of week
	packet[9] = hour; // Hours
	packet[10] = minute; // Minutes
	packet[11] = second; // Seconds
	packet[12] = 0; // CRC MSB
	packet[13] = 0; // CRC LSB

	sendPacket(packet, nPacketBytes, 0, 0);
}

/* vibrate() sends the SET_VIBRATE_MODE message to the MetaWatch
	onTime is how long the vibrate motor goes for (in milliseconds)
	offTime is the time the motor is not vibrating (milliseconds)
	numCycles is how many times the On/Off cycle repeats
	
	e.g. vibrate(250, 250, 5) will vibrate for 250ms, then be off for 250ms, and repeat that 5 times
*/
void SFE_MetaWatch::vibrate(unsigned int onTime, unsigned int offTime, unsigned char numCycles)
{
	int nPacketBytes = 12;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_SET_VIBRATE_MODE; // Set vibrate mode msg
	packet[3] = 0; // options (not used)
	packet[4] = 1; // enable
	packet[5] = (onTime & 0xFF);  // on time LSB
	packet[6] = (onTime & 0xFF00) >> 8; // on time MSB
	packet[7] = (offTime & 0xFF);  // off time LSB
	packet[8] = (offTime & 0xFF00) >> 8; // off time MSB
	packet[9] = numCycles;
	packet[10] = 0; // CRC MSB
	packet[11] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);
}

/* fullscreen() tells the watch whether the Arduino will draw the full screen or just the bottom 2/3rd
	full should be 0 or 1. If 0, the watch should draw the top 1/3 of the screen.
		If 1, the Arduino has control over the whole thing.
*/
void SFE_MetaWatch::fullScreen(unsigned char full)
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_CONTROL_FULL_SCREEN; // Set vibrate mode msg
	packet[3] = full; // 0 watch draws top 1/3, 1 Arduino draws full screen
	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);
}

/* update() sends the UPDATE LCD Display (0x43) command the display
	This message can set the display mode or idle mode page to be drawn on the lCD
	page (only applicable if mode is set to idle)
	start is the row on the display to start drawing (0-95) (96 x 96 display)
	end is the last row to be drawn (1-96)
	style sets the UI style to either 3-row or 4-quad mode
	buffer specifies which type of the buffer's data will be drawn (0=mode, 1=page)
	mode sets one of the four modes to be displayed (idle, app, notification, music)
	
	Most of these parameters have default values, if you don't want to bother with them:
	start=0, end=96, style=1, buffer=1, mode=0, which will draw first page of idle mode
*/
void SFE_MetaWatch::update(unsigned char page, unsigned char start, unsigned char end, unsigned char style, unsigned char buffer, unsigned char mode)
{
	int nPacketBytes = 8;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_UPDATE_LCD; // Set vibrate mode msg
	packet[3] = ((style & 0x01) << 7) | ((buffer & 0x01) << 5) | ((page & 0x03) << 2) | (mode & 0x03); // options: gen1(?), buffer mode, page 0, idle mode
	packet[4] = start; // Start row (0-95)
	packet[5] = end; // End row (0-95)
	packet[6] = 0; // CRC MSB
	packet[7] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);
}

/* setBacklight() sends the SET_BACKLIGHT (0x5D) message
	set: 0 turns the BL off, 1 turns it on (for about 5 seconds)
*/
void SFE_MetaWatch::setBacklight(unsigned char set)
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_SET_BACKLIGHT; // Set backlight msg
	packet[3] = set; // 1=on, 0=off
	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);
}

/* setWidget() sends the Set Widget List Message (0xA1)
	msgTotal - total messages. Unclear what exactly this should be.
	msgIndex - Index of the message. Also unclear; related to the msgTotal variable?
	widgIdSet is an array of up to 14 bytes, which can configure up to 7 widgets.
		each widget requires two bytes of data: an ID and a setting.
		WidgetID consists of two 4-bit nibbles of data. The lower bits are a unique ID between 0 and 15.
			The upper 4 bits can be used to set the widget to a pre-defined drawing.
			e.g. if the widget is a clock 0x50 will set widget ID 0 to the logo clock, 0x30 will set it to a different clockface
	numWidg is the number of widgets specified in widgIDSet, should be between 1 and 7
		THis should equal the size of widgIDSet times two.
	
	After sending this command, you'll still need to update the display (I think) by calling update().
*/
// TODO: still working out the kinks in this function. Maybe there's a better way to send widget ID/settings?
//	Not sure if we're required to send 14 widgets at once, or if less is OK?
// 	Documentation on this message is a little hazy.
void SFE_MetaWatch::setWidget(unsigned char msgTotal, unsigned char msgIndex, unsigned char * widgIDSet, unsigned char numWidg)
{
	//int nPacketBytes = 8 + msgTotal*2;
	int nPacketBytes = 6 + numWidg*2;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_WIDGET_LIST; // Set vibrate mode msg
	packet[3] = ((msgTotal & 0x03) << 2) | (msgIndex & 0x03);
	for (int i=0; i<numWidg*2; i++)
	{
		packet[4+i] = widgIDSet[i];
	}
	
	packet[nPacketBytes-2] = 0; // CRC MSB
	packet[nPacketBytes-1] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);
}

/* clear() sends the Load Template message (0x44)
	Can be used to clear the screen either white or black.
	clear(1) turns the screen black, clear(0) clears it white.
*/
void SFE_MetaWatch::clear(unsigned char black)
{
	int nPacketBytes = 7;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_LOAD_TEMPLATE; // Set vibrate mode msg
	packet[3] = MODE_IDLE; // idle mode
	packet[4] = 0x01 & black;	// Options 1=black, 0=white
	packet[5] = 0; // CRC MSB
	packet[6] = 0; // CRC LSB
	  
	sendPacket(packet, nPacketBytes, 0, 0);
}

/* readBattery() sends the Get Battery Status Message (0x56)
	It'll get the response, and update these variables accordingly:
	clipAttached - 0 or 1 that says if the charger is attached
	batteryCharging - 0 or 1 that says if the battery is charging
	batteryCharge - 0-100 percentage of battery left.
	batteryVoltage - The battery voltage in mV (e.g. 3720 mV = 3.72V)
*/
// TODO: This function should probably return something (percentage), rather
//	than just update some member variables (bad form!).
void SFE_MetaWatch::readBattery()
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];
	unsigned char rsp[12];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_GET_BATTERY; // Set vibrate mode msg
	packet[3] = 0x00; // options (not used)

	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, rsp, 12);
	
	clipAttached = rsp[4];
	batteryCharging = rsp[5];
	batteryCharge = rsp[6];
	batteryVoltage = (rsp[9] << 8) | rsp[8];
}

/* reset() tells the watch to reset()
	Watch will reset immediately upon receipt of message.
*/
// This command is undocumented, found it in the source code. Might have other functionality?
void SFE_MetaWatch::reset()
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_RESET; // Set vibrate mode msg
	packet[3] = 0x00; // options (not used)
	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);
	
}

/* sendPacket() is called by just about every other member function. It calculates 
	some CRC bytes, then sends the message string.
	If a response is requested, it'll return that in the response array. Otherwise
	that and the responseLength variable should be 0.
	
	If you're using a bluetooth module that's not the RN-42, this'd be the place
	to modify.
*/
void SFE_MetaWatch::sendPacket(unsigned char * data, int length, unsigned char * response, int responseLength)
{
	int crc = ComputeCRC(data, length - 2);	// Get the crc values for our string
	data[length-1] = (crc & 0xFF00) >> 8;	// LSB goes first
	data[length-2] = crc & 0xFF;	// the MSB

	// If you want a response, let's flush out the bt buffer first.
	if (responseLength > 0)
		bt.flush();
	
	// Send the data out to the BlueSMiRF
	for (int i=0; i<length; i++)
	{
		bt.write(data[i]);
	}
	
	// If a response was requested, read that into the response array.
	if (responseLength > 0)
	{
		delay(BLUETOOTH_RESPONSE_DELAY);
		int i=0;
		while (bt.available() && (i < responseLength))
		{
			response[i++] = bt.read();
		}
	}
}

/* echoMode() will set up an echo interface betwen bluetooth and the Arduino hardware serial
	This is mostly useful if you're having trouble connecting from the BlueSMiRF to MetaWatch.
	
	Anything sent to the Arduino's hardware serial (e.g. entered into the Serial Monitor)
	will be echoed out to the software serial (bluetooth module). ANything sent to the bluetooth
	module will be echoed out of the Arduino's hardware serial (e.g. visible in the Serial monitor).
	
	It's not recommended to try to send message packets to the watch using this echo mode (unless
	you're some kind of genius CRC calculator).
	
	To exit echo mode, type ~~~.
*/
void SFE_MetaWatch::echoMode()
{
	int c;
	int tildeCount = 0;
	Serial.println("Echo mode. Press ~~~ to escape.");

	while(tildeCount < 3)
	{
		if (bt.available())
		{
			c = bt.peek();
			Serial.write(bt.read());
			if (c == '~') tildeCount++;
		}
		if (Serial.available())
		{
			c = Serial.peek();
			bt.write(Serial.read());
			if (c == '~') tildeCount++;
		}
	}
	Serial.println("Exiting echo mode...");
}

/* 
// TODO: This function needs some testing. Message not documented in API, found in source code.
void SFE_MetaWatch::drawClockWidget(unsigned char clockId)
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_DRAW_CLOCK; // Set vibrate mode msg
	packet[3] = clockId; // options
	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);	
}*/

/* 
// TODO: This function needs some testing. Message not documented in API, found in source code.
void SFE_MetaWatch::updateClock()
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_UPDATE_CLOCK; // Set vibrate mode msg
	packet[3] = 0; // options
	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);	
}*/

/* 
// TODO: This function needs some testing. Message not documented in API, found in source code.
void SFE_MetaWatch::idleUpdate()
{
	int nPacketBytes = 6;
	unsigned char packet[nPacketBytes];

	packet[0] = 0x01; // Start byte
	packet[1] = nPacketBytes;
	packet[2] = MSG_IDLE_UPDATE; // Set vibrate mode msg
	packet[3] = 0; // options
	packet[4] = 0; // CRC MSB
	packet[5] = 0; // CRC LSB
	
	sendPacket(packet, nPacketBytes, 0, 0);	
}*/

/* Ugly CRC computing stuff is below........
	This code was found in this nifty python MetaWatch emulator:
	https://github.com/leoluk/metawatch-simulator
	
	Which looks like it was in turn taken from a handy online
	CRC calculator: http://zorc.breitbandkatze.de/crc.html.
	
	Woo! Worked first try. Thanks guys!
*/
int SFE_MetaWatch::ComputeCRC(unsigned char * packet, int nBytes)
{//CRC code taken from here:  http://zorc.breitbandkatze.de/crc.html

	int i;
	unsigned long bit, crc;


	// at first, compute constant bit masks for whole CRC and CRC high bit

	crcmask = ((((unsigned long)1<<(order-1))-1)<<1)|1;
	crchighbit = (unsigned long)1<<(order-1);


	// compute missing initial CRC value

	if (!direct)
	{
		crcinit_nondirect = crcinit;
		crc = crcinit;
		for (i=0; i<order; i++) 
		{
			bit = crc & crchighbit;
			crc<<= 1;
			if (bit) crc^= polynom;
		}
		crc&= crcmask;
		crcinit_direct = crc;
	}
	else 
	{
		crcinit_direct = crcinit;
		crc = crcinit;
		for (i=0; i<order; i++) 
		{
			bit = crc & 1;
			if (bit) crc^= polynom;
			crc >>= 1;
			if (bit) crc|= crchighbit;
		}	
		crcinit_nondirect = crc;
	}

	int c2=crcbitbybitfast((unsigned char *)packet, nBytes);

	return(c2);

}

// fast bit by bit algorithm without augmented zero bytes.
// does not use lookup table, suited for polynom orders between 1...32.
unsigned long SFE_MetaWatch::crcbitbybitfast(unsigned char* p, unsigned long len) 
{
	unsigned long i, j, c, bit;
	unsigned long crc = crcinit_direct;

	for (i=0; i<len; i++) 
	{
		c = (unsigned long)*p++;
		if (refin) c = reflect(c, 8);

		for (j=0x80; j; j>>=1) 
		{
			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) bit^= crchighbit;
			if (bit) crc^= polynom;
		}
	}	

	if (refout) crc=reflect(crc, order);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}

// reflects the lower 'bitnum' bits of 'crc'
unsigned long SFE_MetaWatch::reflect(unsigned long crc, int bitnum) 
{
	unsigned long i, j=1, crcout=0;

	for (i=(unsigned long)1<<(bitnum-1); i; i>>=1) 
	{
		if (crc & i) crcout|=j;
		j<<= 1;
	}
	
	return (crcout);
}