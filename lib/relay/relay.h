#ifndef _RELAY_H_
#define _RELAY_H_
#include <Arduino.h>
#include "memory.h"

class relay
{
private:
    boolean sw_mode;
    byte pin;
    boolean state;

public:
    void init(byte pin);
    void on();
    void off();
    bool toggle();
    void rlswitch(byte swmode);
    String getStateString()
    {
        if (state == LOW)
        {
            return "off";
        }
        return "on";
    }
    bool getState()
    {
        return state;
    }
};

class relays
{
private:
    const int pins[4] = {25, 26, 33, 32};
    relay data[QUANTITYRELAYS];

public:
    relays();
    ~relays();
    void setOnRelayByIndex(int index);
    void setOffRelayByIndex(int index);
    bool toggleRelayByIndex(int index);
    bool getRelayStateByIndex(int index);
    bool *getRelaysState()
    {
        static bool states[QUANTITYRELAYS];
        for (int i = 0; i < QUANTITYRELAYS; i++)
        {
            states[i] = data[i].getState();
        }
        return states;
    }
    String getRelayStateByIndexString(int index);
};

#endif