#ifndef _BUTTOMS_H_
#define _BUTTOMS_H_

#include <Arduino.h>
#include "memory.h"
#include "relay.h"
#include "serverHandle.h"
#include "memory.h"
#include "rtc.h"

#define WORKING 3
#define START 2
#define STOP 4
#define ENABLE 1
#define NONE 0

class buttom
{
private:
    gpio_num_t pin;
    unsigned long pressed;
    unsigned long button_time = 0;
    unsigned long last_button_time = 0;
    int state = NONE;
    unsigned long workingMillis = 0;

public:
    void init(gpio_num_t _pin);
    ~buttom(){

    };
    gpio_num_t getPin()
    {
        return pin;
    }
    bool isPressed()
    {
        return pressed > 0;
    }
    int getState()
    {
        return state;
    }
    unsigned long getMillis()
    {
        return pressed;
    }
    unsigned long getWorkingMillis()
    {
        return workingMillis;
    }
    void setWorkingMillis(unsigned long millis)
    {
        workingMillis = millis;
    }
    void setState(int _state)
    {
        state = _state;
    }
    void enable_buttom();
    static void IRAM_ATTR isr(void *data);
};

class buttoms
{
private:
    const int pins[4] = {34, 35, 36, 39};
    buttom Buttoms[QUANTITYRELAYS];
    relays *Relays;
    serverHandle *ServerHandle;
    memory *Memory;
    RTC *Rtc;

    void handleSendButtomState(int index, boolean state);
    boolean handleStart(buttom *buttom, int time, int relay, unsigned long *timeStamp);
    boolean handleWorking(buttom *buttom, int time, int relay, unsigned long *timeStamp);
    boolean handleStop(buttom *buttom, int time, int relay, unsigned long *timeStamp);
    void handleDelay(buttom *buttom, int index, unsigned long *timeStamp);
    void handlePressAction(buttom *buttom);

public:
    buttoms(relays *_Relays, serverHandle *_ServerHandle, memory *_Memory, RTC *_Rtc);
    void init();
    ~buttoms(){

    };
    buttom *getButtonByIndex(int index)
    {
        return &Buttoms[index];
    }
    buttom *getButtoms()
    {
        return Buttoms;
    }
    void checkButtoms();
};

#endif