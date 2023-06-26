#include "rtc.h"

void RTC::init()
{
    if (!this->DS1307_RTC.begin())
    {
        while (1)
            ;
    }
    initClock();
}

void RTC::initClock()
{
    DateTime now = this->DS1307_RTC.now();
    rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());
}

void RTC::configDateTime(int unixTimeStamp)
{
    this->DS1307_RTC.adjust(DateTime(unixTimeStamp));
    initClock();
}