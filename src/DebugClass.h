/********************************************************************************​********
Copyright by Thomas Kühnert. All rights reserved.

This file is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This file is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details. https://www.gnu.org/licenses/lgpl-3.0.en.html
********************************************************************************​*********/
#ifndef _DEBUG_
#define _DEBUG_
#include <Arduino.h>
#include <Print.h>

// für ESP8266 und ESP32
#if defined(ESP8266)
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#elif defined(ESP32)
#include <WiFi.h>
#else
#error Only for ESP8266 or ESP32
#endif



#define TELNET_PORT 23
#define BUFFER_LEN 128

// Mit diesen Makros ist der Loop ohne verbundenen Client wesentlich schneller (10 µs)
// gegenüber dem direkten Aufruf (36 µs)
#define debPrintf(...) if (Debug.isConnected()) Debug.printf(__VA_ARGS__)
#define debPrint(...) if (Debug.isConnected()) Debug.print(__VA_ARGS__)
#define debPrintln(...) if (Debug.isConnected()) Debug.println(__VA_ARGS__)


class DebugClass : public Print
{
    public:
    DebugClass();
      void begin(String hostName);
    void handle();
    void setRebootEnabled(bool b);
    bool getRebootEnabled();
    bool isConnected();
    // die Print-Funktionen
    virtual size_t write(uint8_t);
  
  private:
    void sendHelp();
    void setHelp();
    void processCommand();
    String _hostName;
    String _vor;
    String _com;
    String _help;
    bool _isConnected = false;
    bool _rebootEnabled = false;
    String _command;

};  

extern DebugClass Debug;
#endif
