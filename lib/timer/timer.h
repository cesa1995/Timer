#ifndef _TIMER_H_
#define _TIMER_H_

#include "memory.h"
#include "wifiHandle.h"
#include "rtc.h"
#include "relay.h"
#include "buttom.h"

#define STARTWORK 1
#define STOPWORK 0
#define NOTHING 2

class timer
{
private:
    memory *Memory;
    wifiHandle *WifiHandle;
    ESP32Time *rtcEsp;
    relays *Relays;
    buttoms *Buttoms;
    const int SECONDSOFDAY = 86400;

public:
    timer(memory *_memory, wifiHandle *_wifiHandle, RTC *_rtcEsp, relays *_relays, buttoms *_Buttoms);
    int getSeconds(long unixTime);
    void timerCheckTime();

private:
    void checkRelay(TimersConfigArray relay, int *currentTime, int index);
    int checkTimer(TimerConfig *timer, int *currentTime, buttom *buttom);
    int forward(int currentSeconds, int startSeconds, int endSeconds);
    int back(int currentSeconds, int startSeconds, int endSeconds);
    int checkTimeValues(int currentSeconds, int startSeconds, int endSeconds);
    int checkInterval(TimerConfig *timer, int *currentTime, buttom *buttom);
};

#endif