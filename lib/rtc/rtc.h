#ifndef _RTC_H_
#define _RTC_H_
#include "RTClib.h"
#include <ESP32Time.h>

class RTC
{
private:
    RTC_DS1307 DS1307_RTC;
    ESP32Time rtc;

public:
    RTC() {}

    void init();
    void initClock();
    void configDateTime(int unixTimeStamp);

    DateTime getTime()
    {
        DateTime now = DS1307_RTC.now();
        return now;
    }
    String timeToString()
    {
        return rtc.getTime() + " " + (String)rtc.getDay() + "/" + (String)rtc.getMonth() + "/" + (String)rtc.getYear();
    }
    ESP32Time *getEspRtc()
    {
        return &rtc;
    }
};
#endif