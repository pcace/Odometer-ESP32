
#include <Arduino.h> // import OTA

void secondsToHMS(const uint32_t seconds, uint16_t &h, uint8_t &m, uint8_t &s)
{
    uint32_t t = seconds;

    s = t % 60;

    t = (t - s) / 60;
    m = t % 60;

    t = (t - m) / 60;
    h = t;
}