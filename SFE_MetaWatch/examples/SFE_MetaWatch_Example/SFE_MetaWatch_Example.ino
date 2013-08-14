/* SFE_MetaWatch_Example
  by: Jim Lindblom
  SparkFun Electronics
  date: August 14, 2013
  license: Beerware. Please use, reuse, and modify this code. If
  you find it useful, and we meet someday, buy me a beer.
  
  A no serial necessary (hopefully) MetaWatch example. Want
  to control your MetaWatch without ever opening up the Serial
  Monitor? Here's some skeleton code to get you started.
  
  Before you upload: there are a few variables that need setting:
  - metaWatchAddress: This should match the BT address of your 
  MetaWatch -- click button D on the watch, and the address
  is the xxxx-xxxx-xxxx formatted number at the bottom. Don't put
  dashes in the metaWatchAddress variable.
  - btBaudRate: Set this variable to the baud rate of your 
  BlueSMiRF (defaults to 115200, but 9600 works better).
  - year, month, date, day, hours, minutes, seconds: Pretty 
  obvious what these do. Follow by example of how they're 
  currently set.
  
  Upon start up, the Arduino will try to connect to the MetaWatch
  (via a BlueSMiRF). Then it'll set the time (as defined in the
  variables below), and update the clock display. The reset is
  up to you. Want to use a light-sensor to control the watch's
  backlight? Or maybe a button press on the Arduino should control
  the watch's vibration motor. Have fun! For help using the
  SFE_MetaWatch library, check out the readme included with it.
*/

#include <SoftwareSerial.h>
#include <SFE_MetaWatch.h>

char metaWatchAddress[] = "0018342F9B56"; // Watch BT address
unsigned long btBaudRate = 115200; // BlueSMiRF baud rate

unsigned int year = 2013;
unsigned char month = 8;
unsigned char date = 13;
unsigned char day = TUESDAY;
unsigned char hours = 16;
unsigned char minutes = 0;
unsigned char seconds = 0;
 
unsigned char fullClockWidget[8] = 
{0x50, CLOCK_WIDGET | WIDGET_PAGE0 | WIDGET_4Q,  // Logo
 0x31, CLOCK_WIDGET | WIDGET_PAGE1 | WIDGET_4Q,  // Big
 0x42, CLOCK_WIDGET | WIDGET_PAGE2 | WIDGET_4Q,  // Fish
 0x23, CLOCK_WIDGET | WIDGET_PAGE3 | WIDGET_4Q}; // Hanzi

SFE_MetaWatch watch(metaWatchAddress, btBaudRate);

void setup()
{
  //Serial.begin(9600);
  watch.begin();
  int connectStatus = watch.connect();
  /*if (connectStatus < 0)
  {
    Serial.print("Watch connect error: ");
    Serial.println(connectStatus);
    Serial.println("Dropping you into echo mode, for manual connect");
    Serial.println();
    Serial.println("Type $$$ (No line ending) to get into command mode, should respond CMD");
    Serial.println("Type C,<BT address><enter> (newline) to connect to watch. No dashes in address.");
    Serial.println("Type ~~~ to exit echo mode");
    watch.echoMode();
  }
  Serial.println("Connect successful!");*/
  delay(2000);
  updateDisplay();
}

void loop()
{
}

void updateDisplay()
{
  watch.setTime(year, month, date, day, hours, minutes, seconds);
  delay(500);
  watch.fullScreen(1);  // Draw on full screen
  delay(500);
  watch.setWidget(1, 0, fullClockWidget, 4); // Setup four clock widgets
  delay(500);
  watch.update(0, 0, 96, 0, 1, 0); // Update the display
  delay(500);
  // Then press button B to cycle through clock faces
}
