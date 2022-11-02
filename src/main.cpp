/**
 * @file main.cpp
 * @author mail@johannes-froelich.de
 * @brief WORKINPROGRESS! a bicyclie / motorcycle tripComputer with Wifi interface
 *
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Wifi.h>       // standard library
#include <WebServer.h>  // standard library
#include "SuperMon.h"   // .h file that stores your html page code
#include <ArduinoOTA.h> // import OTA
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <NMEAGPS.h>
#include <Preferences.h>

#include "DebugClass.h"
uint32_t aktMillis, lastMillis, lastMillis_h;
// const uint32_t INTERVALL = 1 * 10 * 1000;  // 1 Minute
const uint32_t INTERVALL = 1000; // 1 Minute

NMEAGPS gps;                 // This parses the GPS characters
gps_fix currentFix, prevFix; // This holds on to the latest values

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

Preferences prefs;

float floatRetrieve[6];

float distA;
float distB;
float totalDist;
float totalTime;
float dailyTime;
float dailyDist;

float speed = 0.00;

int32_t currentLat, currentLon = 0;
int32_t prevLat, prevLon = 0;
bool hadFix = false;

char XML[2048];

char buf[512];
IPAddress Actual_IP;

IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

WebServer server(80);
void ButtonResetDistA()
{

  Debug.println("ButtonA press-++++++++++++++++++++++++++++++++");
  distA = 0;
  server.send(200, "text/plain", ""); // Send web page
}

void ButtonResetDistB()
{

  Debug.println("ButtonB press-++++++++++++++++++++++++++++++++");
  distB = 0;
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
    // sprintf(buf, "<SPEED>%.02f</SPEED>\n", speed);
    sprintf(&buf[strlen(buf)], "<SPEED>%.02f</SPEED>\n", speed);

    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<SPEED>no Fix</SPEED>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<DISTA>%.02f</DISTA>\n", distA);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DISTA>no Fix</DISTA>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<DISTB>%.02f</DISTB>\n", distB);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DISTB>no Fix</DISTB>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<TOTALDIST>%.02f</TOTALDIST>\n", totalDist);
    // sprintf(buf, "<TOTALDIST>%d.%d</TOTALDIST>\n", (int)(totalDist), abs((int)(totalDist * 10) - ((int)(totalDist)*10)));
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
    sprintf(buf, "<DAILYDIST>%.02f</DAILYDIST>\n", dailyDist);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<DAILYDIST>no Fix</DAILYDIST>\n");
    strcat(XML, buf);
  }

  if (currentFix.valid.location)
  {
    sprintf(buf, "<POSITION>%d, %d</POSITION>\n", currentLat, currentLon);
    strcat(XML, buf);
  }
  else
  {
    sprintf(buf, "<POSITION>no position Fix</POSITION>\n");
    strcat(XML, buf);
  }

  strcat(XML, "</Data>\n");
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
  prefs.begin("FloatArray");
  prefs.getBytes("FloatArray", &floatRetrieve, sizeof(floatRetrieve));

  distA = floatRetrieve[0];
  distB = floatRetrieve[1];
  totalDist = floatRetrieve[2];
  totalTime = floatRetrieve[3];
  dailyTime = floatRetrieve[4];
  dailyDist = floatRetrieve[5];
  // uint8_t counter = 0;
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
  aktMillis = millis();
  ArduinoOTA.handle();

  while (gps.available(Serial2))

  {
    currentFix = gps.read();

    if (currentFix.speed_kph() > 0) // filter out everything below 1km/h
    {
      speed = currentFix.speed_kph(); //> 1 ? currentFix.speed_kph() : 0;
      Debug.print("speed: ");
      Debug.println(speed);
      // Serial.println(speed);

      Debug.print("Sattelites: ");
      Debug.println(currentFix.satellites);

      Debug.print("hdop");
      Debug.println(currentFix.hdop);
      Debug.print("vdop");
      Debug.println(currentFix.vdop);

      if (currentFix.valid.location)
      {
        currentLat = currentFix.location.lat();
        currentLon = currentFix.location.lon();

        if ((currentLat != prevLat) && (currentLon != prevLon))
        {
          if (hadFix) // prevent 0 prevFix with current location the first time it has a fix
          {
            float distance = currentFix.location.DistanceKm(prevFix.location);
            distA = distA + distance;
            distB = distB + distance;
            totalDist = totalDist + distance;
          }
          prevLat = currentLat;
          prevLon = currentLon;
          prevFix = currentFix;
          hadFix = true;
          Debug.print(distA);
          Debug.println(F(" km"));
        }
      }
      else
      {
        Debug.println(", no Location!");
      }

      if (aktMillis - lastMillis >= INTERVALL)
      {
        // save all Values to Pref file every second... stupid? idk...
        totalTime = totalTime + (1 / 60 / 60);
        float floatStore[6] = {distA, distB, totalDist, totalTime, dailyTime, dailyDist};
        prefs.putBytes("FloatArray", (byte *)(&floatStore), sizeof(floatStore));

        lastMillis = aktMillis;
      }
    }
  }
  // Debug-Handler
  Debug.handle();
  server.handleClient();
}
