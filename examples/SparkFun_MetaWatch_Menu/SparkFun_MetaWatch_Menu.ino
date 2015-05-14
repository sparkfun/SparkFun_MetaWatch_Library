/* SparkFun_MetaWatch Library Serial Menu Example
  by: Jim Lindblom
  SparkFun Electronics
  date: August 14, 2013
  license: Beerware. Please use, reuse, and modify this code. If
  you find it useful, and we meet someday, buy me a beer.
  
  Before uploading: Correctly set the metaWatchAddress[] variable to the BT
  address of your MetaWatch -- click button D on the watch the address
  is the xxxx-xxxx-xxxx formatted number at the bottom. Don't put
  dashes in the metaWatchAddress variable.
  Also set the btBaudRate variable to the baud rate of your 
  BlueSMiRF (defaults to 115200, 9600 works better).
  Also set the year, month, date, and day variables, as there's not a
  menu function to set those currently. They'll be updated when you
  set the time.
  
  After uploading this code, open up your serial monitor and follow the
  directions. Initially, you can press 'k' to try to connect to
  the watch. If that fails, you'll enter echo mode, where you can
  try to connect manually. If connect is successful, test out some
  of the menu options. Here's a few we recommend:
  
    * w - Update WIDGETS: This will update the four idle pages of the display
      to show four full-face clock options. A commented out piece of code
      can be swapped out to show other functionality of the setWidget function.
      After sending this command, pruss button B to cycle through pages.
    * t - set TIME: After sending this command type six numbers for HH:MM:SS
      time should be of the 24-hour variety.
    * v - VIBRATE: Whoo, good vibrations.
    * l and L - BACKLIGHT: Turn the BL off or on (for a few seconds)
	
	**Updated for Arduino 1.6.4 5/2015**
*/

#include <SoftwareSerial.h>
#include <SparkFun_MetaWatch.h>

char metaWatchAddress[] = "0018342F9B56";
unsigned long btBaudRate = 115200;

unsigned int year = 2013;
unsigned char month = 8;
unsigned char date = 14;
unsigned char day = WEDNESDAY;
unsigned char hours = 4;
unsigned char minutes = 9;
unsigned char seconds = 0;

// This widget option sets up four clocks on four different
// idle pages. These are the default full-page clocks 
// programmed into the MetaWatch's memory.
unsigned char fullClockWidget[8] = 
{0x50, CLOCK_WIDGET | WIDGET_PAGE0 | WIDGET_4Q,  // Logo
 0x31, CLOCK_WIDGET | WIDGET_PAGE1 | WIDGET_4Q,  // Big
 0x42, CLOCK_WIDGET | WIDGET_PAGE2 | WIDGET_4Q,  // Fish
 0x23, CLOCK_WIDGET | WIDGET_PAGE3 | WIDGET_4Q}; // Hanzi
 
// This widget option sets up 7 different clocks across four
// idle pages. Four quarter-clocks on the first page. Then
// one clock on the remaining pages.
unsigned char widgIDs[14] =
{0x0, CLOCK_WIDGET | WIDGET_PAGE0 | WIDGET_POS0,
 0x1, CLOCK_WIDGET | WIDGET_PAGE0 | WIDGET_POS1,
 0x2, CLOCK_WIDGET | WIDGET_PAGE0 | WIDGET_POS2,
 0x3, CLOCK_WIDGET | WIDGET_PAGE0 | WIDGET_POS3,
 0x4, CLOCK_WIDGET | WIDGET_PAGE1 | WIDGET_POS1,
 0x5, CLOCK_WIDGET | WIDGET_PAGE2 | WIDGET_POS2,
 0x6, CLOCK_WIDGET | WIDGET_PAGE3 | WIDGET_POS3};
 
// Must call this to initialize the library. Refer to the watch variable
// from here on out.
SFE_MetaWatch watch(metaWatchAddress, btBaudRate);

void setup()
{
  Serial.begin(9600);
  watch.begin();  // This starts up the SoftwareSerial library for bluetooth
  Serial.println("If you're not connected yet, press 'k' to try");
  Serial.println("...otherwise press any key to get the menu");
  while (!Serial.available())
    ;
  if (Serial.read() == 'k') // If we receive k, try to connect.
  {
    Serial.println("Attempting connection...");
    if (watch.connect() < 0) // If connect fails, enter echo mode to manually connect
    {
      Serial.println("Connect failure. Putting you into echo mode.");
      Serial.println("Type $$$ (no line ending) to get into command mode, should respond CMD");
      Serial.println("Type C,<BT address> (newline) to connect to watch. No dashes in address.");
      Serial.println("Type ~~~ to exit echo mode");
      watch.echoMode();  // echo mode, will loop forever until it receives ~~~
    }
  }
  Serial.flush();  // Get rid of anything else in the serial buffer.
  printMenu();  // Print the menu and go to loop()
}

void loop()
{
  while (!Serial.available())
    ;  // Wait for something to come in from the Serial Monitor
  unsigned char c = Serial.read(); // Read it into c
  switch(c)
  {
  case 'e':  // Echo mode
    watch.echoMode();
    break;
  case 't':
    getTime(); // getTime will update the hours, minutes and seconds variables
    // setTime updates the RTC of the MetaWatch. All 7 parameters are required.
    watch.setTime(year, month, date, day, hours, minutes, seconds);
    break;
  case 'v':
    // vibrate's three parameters are: onTime, offTime, and repeats.
    // set to (100, 100, 5) the watch will vibrate for 100ms, stop for 100ms
    // and repeat that process 5 times.
    watch.vibrate(100, 100, 5);
    break;
  case 'b':
    // readBattery() doesn't return anything but it updates a handful
    // of public member variables. Read batteryCharge to get the watch's
    // charge as a %  (0-100)
    watch.readBattery();
    Serial.print("Battery Charge: ");
    Serial.print(watch.batteryCharge);
    Serial.println(" %");
    break;
  case 'C':
    watch.fullScreen((byte) 0);
    watch.clear(0); // Clear the screen white
    watch.update(0, 0, 96, 0, 1, 0); // Have to update display to get it to happen
    Serial.println("Screen should be white");
    break;
  case 'c':
    watch.fullScreen((byte) 0);
    watch.clear(1); // Clear the screen black
    watch.update(0, 0, 96, 0, 1, 0); // Have to update display to get it to happen
    Serial.println("Screen should be black");
    break;
  case 'w':
    // This will send the setWidget command, to update the watch's widgets
    // on it's four idle pages.
    watch.fullScreen(1); // Two options here are 0 for 2/3rd screen or 1 for full-screen
    //watch.setWidget(1, 0, widgIDs, 7); // A different widget-setting option
    // setWidget requires four (mysterious) parameters. The first two:
    // msgIndex and msgTotal seem to work best at 1 and 0.
    // The third should be an array of 2 to 14 bytes, which sets 1 to 7 widgets
    // each widget requires two bytes of data: an ID and a settings byte.
    // See the library and API for more info on these bytes.
    // The fourth paramter sets the number of widgets to be updated (1-7)
    watch.setWidget(1, 0, fullClockWidget, 4);
    watch.update(0, 0, 96, 0, 1, 0); // Update display to make it happen.
    Serial.println("Widgets updated");
    break;
  case 'r':
    watch.reset();  // This fully resets the Watch: widgets and RTC included.
    break;
  case 'L':
    watch.setBacklight(1);  // Turn backlight on for 5 seconds
    break;
  case 'l':
    watch.setBacklight(0); // Turn backlight off
  case 'k':
    watch.connect(); // Attempt to connect BlueSmirf to watch
    break;
  }
}

void getTime()
{
  char msd, lsd;
  // Update Hours
  Serial.println("Type two digits for _hours_");
  while (Serial.available() < 2)
    ;
  msd = Serial.read();
  lsd = Serial.read();
  hours = ((msd - 0x30) * 10) + (lsd - 0x30);
  Serial.print("Hours updated to ");
  Serial.println(hours);
  Serial.flush();
  
  // Update minutes
  Serial.println("Type two digits for _minutes_");
  while (Serial.available() < 2)
    ;
  msd = Serial.read();
  lsd = Serial.read();
  minutes = ((msd - 0x30) * 10) + (lsd - 0x30);
  Serial.print("Minutes updated to ");
  Serial.println(minutes);
  Serial.flush();
  
  // Update seconds
  Serial.println("Type two digits for _minutes_");
  while (Serial.available() < 2)
    ;
  msd = Serial.read();
  lsd = Serial.read();
  seconds = ((msd - 0x30) * 10) + (lsd - 0x30);
  Serial.print("Seconds updated to ");
  Serial.println(seconds);
  Serial.flush();
}

void printMenu()
{
  Serial.println(F("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));
  Serial.println(F("-= MetaWatch BT Config Menu -="));
  Serial.println(F("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));
  Serial.println();
  Serial.println(F("e - ECHO mode"));
  Serial.println(F("t - test SET TIME"));
  Serial.println(F("v - test VIBRATE"));
  Serial.println(F("b - read BATTERY"));
  Serial.println(F("C - clear screen WHITE"));
  Serial.println(F("c - clear screen BLACK"));
  Serial.println(F("w - Update WIDGETs"));
  Serial.println(F("r - RESET"));
  Serial.println(F("l - Turn BACKLIGHT off"));
  Serial.println(F("L - Turn BACKLIGHT on"));
  Serial.println(F("k - CONNECT"));
  
  Serial.println();
}
