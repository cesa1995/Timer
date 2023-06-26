#include "timer.h"

timer::timer(memory *_memory, wifiHandle *_wifiHandle, RTC *_rtcEsp, relays *_Relays, buttoms *_Buttoms)
{
    Memory = _memory;
    WifiHandle = _wifiHandle;
    rtcEsp = _rtcEsp->getEspRtc();
    Relays = _Relays;
    Buttoms = _Buttoms;
}

int timer::getSeconds(long unixTime)
{
    int numberOfDay = floor(unixTime / SECONDSOFDAY);
    return unixTime - (numberOfDay * SECONDSOFDAY);
}

void timer::timerCheckTime()
{
    int currentEspTime = getSeconds(this->rtcEsp->getEpoch());
    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        checkRelay(Memory->getTimersByRelay(i), &currentEspTime, i);
    }
}

void timer::checkRelay(TimersConfigArray Relay, int *currentTime, int index)
{
    for (int i = 0; i < *Relay.size; i++)
    {
        int STEP = checkTimer(&Relay.timers[i], currentTime, Buttoms->getButtonByIndex(index));
        if (STEP == STARTWORK)
        {
            if (!Relays->getRelayStateByIndex(index))
            {
                Relays->setOnRelayByIndex(index);
            }
            return;
        }
        if (STEP == STOPWORK)
        {
            if (Relays->getRelayStateByIndex(index))
            {
                Relays->setOffRelayByIndex(index);
            }
        }
    }
}
int timer::checkTimer(TimerConfig *timer, int *currentTime, buttom *buttom)
{
    if (!timer->enableTimer)
    {
        return NOTHING;
    }
    return checkInterval(timer, currentTime, buttom);
}

int timer::forward(int currentSeconds, int startSeconds, int endSeconds)
{
    if (currentSeconds > startSeconds && currentSeconds < endSeconds)
    {
        return endSeconds - startSeconds;
    }
    return 0;
}

int timer::back(int currentSeconds, int startSeconds, int endSeconds)
{
    if (currentSeconds < startSeconds && currentSeconds > endSeconds)
    {
        return 0;
    }
    return startSeconds + endSeconds;
}

int timer::checkTimeValues(int currentSeconds, int startSeconds, int endSeconds)
{
    if (startSeconds > endSeconds)
    {
        return this->back(currentSeconds, startSeconds, endSeconds);
    }
    return this->forward(currentSeconds, startSeconds, endSeconds);
}

int timer::checkInterval(TimerConfig *timer, int *currentTime, buttom *buttom)
{
    int startTime = getSeconds(timer->startTime);
    int endTime = getSeconds(timer->endTime);
    int timerSpent = *currentTime - startTime;
    if (timerSpent < 5 && timerSpent > 0)
    {
        buttom->setState(NONE);
    }

    if (buttom->getState() != NONE)
    {
        return NOTHING;
    }
    int totalWorkTimer = checkTimeValues(*currentTime, startTime, endTime);
    if (totalWorkTimer == 0)
    {
        return STOPWORK;
    }
    if (timer->intervalWork == 0 || timer->intervalStop == 0)
    {
        return STARTWORK;
    }
    double numbersOfCycles = (double)totalWorkTimer / (double)(timer->cycle);
    double cyclesState = (double)timerSpent * numbersOfCycles / (double)totalWorkTimer;
    int cyclesCompleted = floor(cyclesState);
    double currentCyclePosition = cyclesState - (double)cyclesCompleted;
    if (currentCyclePosition < timer->workcycle)
    {
        return STARTWORK;
    }
    return STOPWORK;
}