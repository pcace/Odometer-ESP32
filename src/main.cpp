#include <Wifi.h>       // standard library
#include <WebServer.h>  // standard library
#include "SuperMon.h"   // .h file that stores your html page code
#include <ArduinoOTA.h> // import OTA
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <NMEAGPS.h>

#include "DebugClass.h"
uint32_t aktMillis, lastMillis, lastMillis_h;
// const uint32_t INTERVALL = 1 * 10 * 1000;  // 1 Minute
const uint32_t INTERVALL = 1000; // 1 Minute

NMEAGPS gps;                 // This parses the GPS characters
gps_fix currentFix, prevFix; // This holds on to the latest values

// Storage:
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

#define USE_INTRANET

#include ".env"
// #define LOCAL_SSID "wifi"
// #define LOCAL_PASS "password"

#define AP_SSID "TestWebSite"
#define AP_PASS "023456789"

#define HOST_NAME "ESP_Tacho"

// variables to store measure data and sensor states

float distA = 1;      // EEPROM Adress: 0
float distB = 2;      // EEPROM Adress: 1
float totalDist = 30; // EEPROM Adress: 2
float totalTime = 04; // EEPROM Adress: 3
float dailyTime = 5;  // EEPROM Adress: 4
float dailyDist = 6;  // EEPROM Adress: 5
float speed = 11;
int32_t currentLat, currentLon = 0;
int32_t prevLat, prevLon = 0;
bool hadFix = false;

char XML[2048];

char buf[256];
IPAddress Actual_IP;

IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

WebServer server(80);
void ButtonResetDistA()
{

  Debug.println("ButtonA press-++++++++++++++++++++++++++++++++");
  distA = distA * 2;
  server.send(200, "text/plain", ""); // Send web page
}

void ButtonResetDistB()
{

  Debug.println("ButtonB press-++++++++++++++++++++++++++++++++");
  server.send(200, "text/plain", ""); // Send web page
}

void SendWebsite()
{

  Debug.println("sending web page");
  server.send(200, "text/html", PAGE_MAIN);
}

void SendXML()
{

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  if (currentFix.valid.speed)
  {
    sprintf(buf, "<SPEED>%.02f</SPEED>\n", speed);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<SPEED>no Fix</SPEED>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<DISTA>%.05f</DISTA>\n", distA);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DISTA>no Fix</DISTA>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<DISTB>%d.%d</DISTB>\n", (int)(distB), abs((int)(distB * 10) - ((int)(distB)*10)));
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DISTB>no Fix</DISTB>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<TOTALDIST>%d.%d</TOTALDIST>\n", (int)(totalDist), abs((int)(totalDist * 10) - ((int)(totalDist)*10)));
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<TOTALDIST>no Fix</TOTALDIST>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.date && currentFix.valid.time)
  {
    sprintf(buf, "<TOTALTIME>%d.%d</TOTALTIME>\n", (int)(totalTime), abs((int)(totalTime * 10) - ((int)(totalTime)*10)));
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<TOTALTIME>no Fix</TOTALTIME>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.date && currentFix.valid.time)
  {
    sprintf(buf, "<DAILYTIME>%d.%d</DAILYTIME>\n", (int)(dailyTime), abs((int)(dailyTime * 10) - ((int)(dailyTime)*10)));
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DAILYTIME>no Fix</DAILYTIME>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<DAILYDIST>%d.%d</DAILYDIST>\n", (int)(dailyDist), abs((int)(dailyDist * 10) - ((int)(dailyDist)*10)));
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DAILYDIST>no Fix</DAILYDIST>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<POSITION>%f, %f</POSITION>\n", currentLat, currentLon);
    // dtostrf(val, 4, 6, buf);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<POSITION>no position Fix</POSITION>\n");
    strcat(XML, buf);
  }

  strcat(XML, "</Data>\n");

  // Serial2.println(XML);

  server.send(200, "text/xml", XML);
}

void printWifiStatus()
{

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("Open http://");
  Serial.println(ip);
}

void setup()
{

  uint8_t counter = 0;

  Serial.begin(9600);
  Serial2.begin(9600);

  disableCore0WDT();

  Serial.println("starting server");

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

#ifndef USE_INTRANET
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);
  WiFi.softAPConfig(PageIP, gateway, subnet);
  delay(100);
  Actual_IP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(Actual_IP);
#endif

  server.on("/", SendWebsite);
  server.on("/xml", SendXML);

  server.on("/BUTTON_0", ButtonResetDistA);
  server.on("/BUTTON_1", ButtonResetDistB);

  server.begin();

  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Debug.begin(HOST_NAME);
}

void loop()
{
  ArduinoOTA.handle();

  while (gps.available(Serial2))

  {
    currentFix = gps.read();

    // speed = currentFix.speed_kph();
    // Debug.print("speed: ");
    // Serial.print("speed: ");
    // Debug.println(speed);
    // // Serial.println(speed);

    // Debug.println("Sattelites");
    // Debug.print(currentFix.satellites);

    // Debug.println("hdop");
    // Debug.print(currentFix.hdop);
    // Debug.println("vdop");
    // Debug.print(currentFix.vdop);

    if (currentFix.valid.location)
    {
      currentLat = currentFix.location.lat();
      currentLon = currentFix.location.lon();
      if ((currentLat != prevLat) && (currentLon != prevLon))
      {
        if (hadFix) // prevent 0 prevFix with current location the first time it has a fix
        {
          Debug.println(distA);
          Debug.println(currentLat);
          Debug.println(prevLat);
          Debug.println(currentLon);
          Debug.println(prevLon);
          distA = distA + currentFix.location.DistanceKm(prevFix.location);
        }
        prevLat = currentLat;
        prevLon = currentLon;
        hadFix = true;
      }

      Debug.print(distA);
      Debug.println(F(" km"));

      // Debug.print("dist: ");
      // Debug.println(distA);
      // Debug.print(F("Location: "));
      // Debug.print(currentFix.latitude(), 6);
      // Debug.print(',');
      // Debug.print(currentFix.longitude(), 6);
    }
    else
    {
      Debug.println(", no Location currentFix!");
      Serial.println(", no Location currentFix!");
    }

    //   if (currentFix.valid.altitude)
    //   {
    //     Debug.print(F(", Altitude: "));
    //     Serial.print(F(", Altitude: "));
    //     Debug.print(currentFix.altitude());
    //     Serial.print(currentFix.altitude());
    //     Debug.println();
    //     Serial.println();
    //   }
    //   else
    //   {
    //     Debug.println(", no Altitude FIX!");
    //     Serial.println(", no Altitude FIX!");
    //   }
  }

  // Debug-Handler
  Debug.handle();
  aktMillis = millis();
  if (aktMillis - lastMillis >= INTERVALL)
  {
    // Debug.print("Debug.print");
    lastMillis = aktMillis;
  }
  server.handleClient();
}
