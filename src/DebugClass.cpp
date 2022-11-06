#include "DebugClass.h"

WiFiServer telnetServer(TELNET_PORT);
WiFiClient telnetClient;
// extern ESP8266WiFiClass WiFi;
DebugClass::DebugClass() {}

String buffer = "";

void DebugClass::begin(String hostName) {
  _hostName = hostName;
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  _help.reserve(120);
  _command.reserve(5);
  _command = "";
  _vor.reserve(_hostName.length()+3);
  _vor="("+_hostName+") ";
  _com.reserve(_vor.length()+22);
  _com=_vor+"Kommando empfangen: ";
  buffer.reserve(BUFFER_LEN);
  buffer = _vor;
  setHelp();
}

void DebugClass::setHelp() {
  _help = "Debug from ";
  _help+=_hostName;
  _help+=" IP: ";
  _help+=WiFi.localIP().toString();
  _help+="\n\rKommandos (mit <ENTER> beenden): q = Quit h = Hilfe s = Speicher r = reset all Values";
  if (_rebootEnabled) _help+=" r = Reboot";
  // Serial.println(_help.length());
  // Serial.println(_help);
}

void DebugClass::handle() {
char c;  
  // neuer Client?
  if (telnetServer.hasClient()) {
    telnetClient = telnetServer.available();
    if (!telnetClient) return;
    Serial.print("Neuer Client: ");Serial.println(telnetClient.remoteIP());
    telnetClient.setNoDelay(true);
    telnetClient.flush();
    sendHelp();
    // puffer leeren
    while(telnetClient.available()) {
      telnetClient.read();
    }
  }
  _isConnected = (telnetClient && telnetClient.connected());
  if (!_isConnected) return;
  while(telnetClient.available()) {
    c = telnetClient.read();
    if (c == '\n' || c == '\r') {
      if (_command.length() > 0) {
        processCommand();
      }
      _command = "";
    }
    else {
      if (c>31 && c<128) _command.concat(c);
    }
  }  
}

bool DebugClass::isConnected() {
  return _isConnected;  
}

void DebugClass::processCommand() {
  telnetClient.print(_com);telnetClient.println(_command);
  // Hilfe
  if (_command == "h") {
    sendHelp();
  }
  // Quit
  else if (_command == "q") {
    telnetClient.print(_vor);
    telnetClient.println("* Closing telnet connection ...");
    delay(5);
    telnetClient.stop();
    _isConnected = false;
  }
  // Speicher
  else if (_command == "s") {
    telnetClient.print(_vor);
    telnetClient.print("* Free Heap RAM: ");
    telnetClient.println(ESP.getFreeHeap());
  }
  // Reboot
  else if (_rebootEnabled && _command == "r") {
    telnetClient.print(_vor);
    telnetClient.println("* Resetting the ESP8266 ...");
    telnetClient.stop();
    telnetServer.stop();
    delay (500);
    // Reset
    ESP.restart();
  }
}

void DebugClass::sendHelp() {
  telnetClient.println(_help);
}

void DebugClass::setRebootEnabled(bool b) {
  _rebootEnabled = b;
  setHelp();
}
bool DebugClass::getRebootEnabled() {
  return _rebootEnabled;
}

size_t DebugClass::write(uint8_t c) {  
size_t ret = 0;  
bool send = false, newline = false;
  if (!_isConnected) return ret;
  if (c == '\n') {
    send = true;
    newline = true;
  }
  else buffer.concat((char)c);
  // Serial.println(buffer);
  if (buffer.length() >= BUFFER_LEN) send = true;
  if (send) {
    if (newline) {
      ret = telnetClient.println(buffer);
      buffer = _vor;
    }
    else {
      ret = telnetClient.print(buffer);
      buffer = "";
    }
  }  
  return ret;
}

DebugClass Debug = DebugClass();