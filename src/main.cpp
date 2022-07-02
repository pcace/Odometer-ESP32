/*


  this example will show
  1. how to use and ESP 32 for reading pins
  2. building a web page for a client (web browser, smartphone, smartTV) to connect to
  3. sending data from the ESP to the client to update JUST changed data
  4. sending data from the web page (like a slider or button press) to the ESP to tell the ESP to do something

  If you are not familiar with HTML, CSS page styling, and javascript, be patient, these code platforms are
  not intuitive and syntax is very inconsitent between platforms

  I know of 4 ways to update a web page
  1. send the whole page--very slow updates, causes ugly page redraws and is what you see in most examples
  2. send XML data to the web page that will update just the changed data--fast updates but older method
  3. JSON strings which are similar to XML but newer method
  4. web sockets very very fast updates, but not sure all the library support is available for ESP's

  I use XML here...

  compile options
  1. esp32 dev module
  2. upload speed 921600
  3. cpu speed 240 mhz
  flash speed 80 mhz
  flash mode qio
  flash size 4mb
  partition scheme default


  NOTE if your ESP fails to program press the BOOT button during programm when the IDE is "looking for the ESP"


*/

#include <WiFi.h>      // standard library
#include <WebServer.h> // standard library
#include "SuperMon.h"  // .h file that stores your html page code

// include GPS library
#include <NMEAGPS.h>
// #include <GPSport.h>
NMEAGPS gps; // This parses the GPS characters
gps_fix fix; // This holds on to the latest values

//Storage: 
#include <EEPROM.h>
#define EEPROM_SIZE 12


#if !defined(NMEAGPS_PARSE_RMC) & \
    !defined(NMEAGPS_PARSE_GGA) & \
    !defined(NMEAGPS_PARSE_GLL)
#error You must uncomment at least one of NMEAGPS_PARSE_RMC, NMEAGPS_PARSE_GGA or NMEAGPS_PARSE_GLL in NMEAGPS_cfg.h!
#endif

#if !defined(GPS_FIX_LOCATION)
#error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

// include this as storage location for later...
#include <EEPROM.h>

// here you post web pages to your homes intranet which will make page debugging easier
// as you just need to refresh the browser as opposed to reconnection to the web server
#define USE_INTRANET

// replace this with your homes intranet connect parameters
#define LOCAL_SSID "LAND_4"
#define LOCAL_PASS "280389cassiopaia"

// once  you are read to go live these settings are what you client will connect to
#define AP_SSID "TestWebSite"
#define AP_PASS "023456789"

// start your defines for pins for sensors, outputs etc.
#define PIN_OUTPUT 26 // connected to nothing but an example of a digital write from the web page
#define PIN_FAN 27    // pin 27 and is a PWM signal to control a fan speed
#define PIN_LED 2     // On board LED
#define PIN_A0 34     // some analog input sensor
#define PIN_A1 35     // some analog input sensor


// variables to store measure data and sensor states

float distA = 1; // EEPROM Adress: 0
float distB = 2; // EEPROM Adress: 1
float totalDist = 30; // EEPROM Adress: 2
float totalTime = 04; // EEPROM Adress: 3
float dailyTime = 5; // EEPROM Adress: 4
float dailyDist = 6; // EEPROM Adress: 5
float speed = 11;
int32_t lat, lon = 0;

// the XML array size needs to be bigger that your maximum expected size. 2048 is way too big for this example
char XML[4096];

// just some buffer holder for char operations
char buf[32];

// variable for the IP reported when you connect to your homes intranet (during debug mode)
IPAddress Actual_IP;

// definitions of your desired intranet created by the ESP32
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// gotta create a server
WebServer server(80);

// function managed by an .on method to handle slider actions on the web page
// this example will get the passed string called VALUE and conver to a pwm value
// and control the fan speed

// now process button_0 press from the web site. Typical applications are the used on the web client can
// turn on / off a light, a fan, disable something etc
void ButtonResetDistA()
{

  Serial.println("ButtonA press-++++++++++++++++++++++++++++++++");
  //
distA = distA*2;
  // Serial2.print("Button 0 ");
  // Serial2.println(LED0);
  // regardless if you want to send stuff back to client or not
  // you must have the send line--as it keeps the page running
  // if you don't want feedback from the MCU--or let the XML manage
  // sending feeback

  // option 1 -- keep page live but dont send any thing
  // here i don't need to send and immediate status, any status
  // like the illumination status will be send in the main XML page update
  // code
  server.send(200, "text/plain", ""); // Send web page

  // option 2 -- keep page live AND send a status
  // if you want to send feed back immediataly
  // note you must have reading code in the java script
  /*
    if (LED0) {
    server.send(200, "text/plain", "1"); //Send web page
    }
    else {
    server.send(200, "text/plain", "0"); //Send web page
    }
  */
}

// same notion for processing button_1
void ButtonResetDistB()
{

  // just a simple way to toggle a LED on/off. Much better ways to do this
  Serial.println("ButtonB press-++++++++++++++++++++++++++++++++");
  // regardless if you want to send stuff back to client or not
  // you must have the send line--as it keeps the page running
  // if you don't want feedback from the MCU--or send all data via XML use this method
  // sending feeback
distB = distB + 3.6323;
  server.send(200, "text/plain", ""); // Send web page

  // if you want to send feed back immediataly
  // note you must have proper code in the java script to read this data stream
  /*
    if (some_process) {
    server.send(200, "text/plain", "SUCCESS"); //Send web page
    }
    else {
    server.send(200, "text/plain", "FAIL"); //Send web page
    }
  */
}

// code to send the main web page
// PAGE_MAIN is a large char defined in SuperMon.h
void SendWebsite()
{

  Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);
}

// code to send the main web page
// I avoid string data types at all cost hence all the char mainipulation code
void SendXML()
{
  /**
   * distA = 0;
  distB = 0;
  totalDist = 0;
  totalTime = 0;
  dailyTime = 0;
  dailyDist = 0;
  speed = 0;
   *
   */
  // Serial2.println("sending xml");

  /**
   * send float: 
   *   sprintf(buf, "<V0>%d.%d</V0>\n", (int) (VoltsA0), abs((int) (VoltsA0 * 10)  - ((int) (VoltsA0) * 10)));
   * 
   */

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");


if(fix.valid.speed) {
  sprintf(buf, "<SPEED>%d.%d</SPEED>\n", (int) (speed), abs((int) (speed * 10)  - ((int) (speed) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<SPEED>no Fix</SPEED>\n");
  strcat(XML, buf);
}

if(fix.valid.location) {
  sprintf(buf, "<DISTA>%d.%d</DISTA>\n", (int) (distA), abs((int) (distA * 10)  - ((int) (distA) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<DISTA>no Fix</DISTA>\n");
  strcat(XML, buf);
}

if(fix.valid.location) {
  sprintf(buf, "<DISTB>%d.%d</DISTB>\n", (int) (distB), abs((int) (distB * 10)  - ((int) (distB) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<DISTB>no Fix</DISTB>\n");
  strcat(XML, buf);
}

if(fix.valid.location) {
  sprintf(buf, "<TOTALDIST>%d.%d</TOTALDIST>\n",  (int) (totalDist), abs((int) (totalDist * 10)  - ((int) (totalDist) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<TOTALDIST>no Fix</TOTALDIST>\n");
  strcat(XML, buf);
}


if(fix.valid.date && fix.valid.time) {
  sprintf(buf, "<TOTALTIME>%d.%d</TOTALTIME>\n",  (int) (totalTime), abs((int) (totalTime * 10)  - ((int) (totalTime) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<TOTALTIME>no Fix</TOTALTIME>\n");
  strcat(XML, buf);
}


if(fix.valid.date && fix.valid.time) {
  sprintf(buf, "<DAILYTIME>%d.%d</DAILYTIME>\n",  (int) (dailyTime), abs((int) (dailyTime * 10)  - ((int) (dailyTime) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<DAILYTIME>no Fix</DAILYTIME>\n");
  strcat(XML, buf);
}


if(fix.valid.location) {
  sprintf(buf, "<DAILYDIST>%d.%d</DAILYDIST>\n", (int) (dailyDist), abs((int) (dailyDist * 10)  - ((int) (dailyDist) * 10)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<DAILYDIST>no Fix</DAILYDIST>\n");
  strcat(XML, buf);
}


if(fix.valid.location) {
  sprintf(buf, "<POSITION>%d.%d, %d.%d</POSITION>\n", (int) (lat), abs((int) (lat * 10000)  - ((int) (lat) * 10000)),  (int) (lon), abs((int) (lon * 10000)  - ((int) (lon) * 10000)));
  strcat(XML, buf);
} else {
    sprintf(buf, "<POSITION>no Fix</POSITION>\n");
  strcat(XML, buf);
}

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  // Serial2.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);
}

// I think I got this code from the wifi example
void printWifiStatus()
{

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Open http://");
  Serial.println(ip);
}

// end of code

void setup()
{

  // initialize GPS


  // Serial.begin(9600, SERIAL_8N1, 25, 27);
  Serial.begin(9600);


  // standard stuff here
  Serial2.begin(9600);

  // if your web page or XML are large, you may not get a call back from the web page
  // and the ESP will think something has locked up and reboot the ESP
  // not sure I like this feature, actually I kinda hate it
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  //  disableCore1WDT();

  // just an update to progress
  Serial.println("starting server");

  // if you have this #define USE_INTRANET,  you will connect to your home intranet, again makes debugging easier
#ifdef USE_INTRANET
  WiFi.begin(LOCAL_SSID, LOCAL_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Actual_IP = WiFi.localIP();
#endif

  // if you don't have #define USE_INTRANET, here's where you will creat and access point
  // an intranet with no internet connection. But Clients can connect to your intranet and see
  // the web page you are about to serve up
#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(Actual_IP);
#endif

  // printWifiStatus();

  // these calls will handle data coming back from your web page
  // this one is a page request, upon ESP getting / string the web page will be sent
  server.on("/", SendWebsite);

  // upon esp getting /XML string, ESP will build and send the XML, this is how we refresh
  // just parts of the web page
  server.on("/xml", SendXML);

  // upon ESP getting /UPDATE_SLIDER string, ESP will execute the UpdateSlider function
  // same notion for the following .on calls
  // add as many as you need to process incoming strings from your web page
  // as you can imagine you will need to code some javascript in your web page to send such strings
  // this process will be documented in the SuperMon.h web page code
  server.on("/BUTTON_0", ButtonResetDistA);
  server.on("/BUTTON_1", ButtonResetDistB);

  // finally begin the server
  server.begin();
}

void loop()
{

  // Serial2.print(F("Location: "));
  while (gps.available(Serial2))
  
  {
    fix = gps.read();

    if (fix.valid.location)
    {

      Serial.print(F("Location: "));
      Serial.print(fix.latitude(), 6);
      Serial.print(',');
      Serial.print(fix.longitude(), 6);
    }
    else
    {
      Serial.println("no Location FIX!");
    }

    if (fix.valid.altitude)
    {
      Serial.print(F(", Altitude: "));
      Serial.print(fix.altitude());
      Serial.println();
    }
    else
    {
      Serial.println("no Altitude FIX!");
    }
  }

  // you main loop that measures, processes, runs code, etc.
  // note that handling the "on" strings from the web page are NOT in the loop
  // that processing is in individual functions all managed by the wifi lib

  // in my example here every 50 ms, i measure some analog sensor data (my finger dragging over the pins
  // and process accordingly
  // analog input can be from temperature sensors, light sensors, digital pin sensors, etc.

  // no matter what you must call this handleClient repeatidly--otherwise the web page
  // will not get instructions to do something
  server.handleClient();
}
