#include "buttom.h"

void buttom::init(gpio_num_t _pin)
{
    pin = _pin;
    attachInterruptArg(pin, isr, this, FALLING);
}

void IRAM_ATTR buttom::isr(void *data)
{
    buttom *_buttom = (buttom *)data;
    _buttom->button_time = millis();
    if (_buttom->button_time - _buttom->last_button_time > 500)
    {
        _buttom->pressed = millis();
        _buttom->last_button_time = _buttom->button_time;
    }
}

void buttom::enable_buttom()
{
    pressed = 0;
}

buttoms::buttoms(relays *_Relays, serverHandle *_ServerHandle, memory *_Memory, RTC *_Rtc)
{
    Relays = _Relays;
    ServerHandle = _ServerHandle;
    Memory = _Memory;
    Rtc = _Rtc;
}

void buttoms::init()
{

    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        Buttoms[i].init((gpio_num_t)pins[i]);
    }
}

void buttoms::checkButtoms()
{
    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        buttom *buttom = getButtonByIndex(i);
        unsigned long timeStamp = Rtc->getEspRtc()->getEpoch();
        if (buttom->isPressed() || ServerHandle->getRelayPress() == i)
        {
            ServerHandle->resetRelayPress();
            buttom->enable_buttom();
            buttom->setWorkingMillis(timeStamp);
            handlePressAction(buttom);
        }
        handleDelay(buttom, i, &timeStamp);
    }
}

void buttoms::handleDelay(buttom *buttom, int index, unsigned long *timeStamp)
{
    int state = buttom->getState();

    if (state == START)
    {
        if (handleStart(buttom, Memory->getButtomConfigByRelay(index).StartDelay, index, timeStamp))
        {
            handleSendButtomState(index, Relays->getRelayStateByIndex(index));
        };
        return;
    }

    if (state == WORKING)
    {
        if (handleWorking(buttom, Memory->getButtomConfigByRelay(index).WorkDelay, index, timeStamp))
        {
            handleSendButtomState(index, Relays->getRelayStateByIndex(index));
        }
        return;
    }

    if (state == STOP)
    {
        if (handleStop(buttom, Memory->getButtomConfigByRelay(index).EndDelay, index, timeStamp))
        {
            handleSendButtomState(index, Relays->getRelayStateByIndex(index));
        }
        return;
    }
}

void buttoms::handlePressAction(buttom *buttom)
{
    if (buttom->getState() == NONE)
    {
        buttom->setState(START);
        return;
    }
    if (buttom->getState() == START)
    {
        buttom->setState(WORKING);
        return;
    }
    if (buttom->getState() == WORKING)
    {
        buttom->setState(STOP);
        return;
    }
    if (buttom->getState() == STOP)
    {
        buttom->setState(ENABLE);
        return;
    }
    if (buttom->getState() == ENABLE)
    {
        buttom->setState(START);
        return;
    }
}

boolean buttoms::handleStart(buttom *buttom, int time, int relay, unsigned long *timeStamp)
{
    if (time == 0)
    {
        Relays->setOnRelayByIndex(relay);
        buttom->setState(WORKING);
        return true;
    }
    if (buttom->getWorkingMillis() < (*timeStamp - time))
    {
        buttom->setWorkingMillis(*timeStamp);
        Relays->setOnRelayByIndex(relay);
        buttom->setState(WORKING);
        return true;
    }
    return false;
}

boolean buttoms::handleWorking(buttom *buttom, int time, int relay, unsigned long *timeStamp)
{
    if (time == 0)
    {
        return false;
    }
    if (buttom->getWorkingMillis() < (*timeStamp - time))
    {
        Relays->setOffRelayByIndex(relay);
        buttom->setState(ENABLE);
        return true;
    }
    return false;
}

boolean buttoms::handleStop(buttom *buttom, int time, int relay, unsigned long *timeStamp)
{
    if (time == 0)
    {
        Relays->setOffRelayByIndex(relay);
        buttom->setState(ENABLE);
        return true;
    }
    if (buttom->getWorkingMillis() < (*timeStamp - time))
    {
        Relays->setOffRelayByIndex(relay);
        buttom->setState(ENABLE);
        return true;
    }
    return false;
}

void buttoms::handleSendButtomState(int index, boolean state)
{
    size_t client = ServerHandle->getWSocket()->count();
    if (client > (size_t)0)
    {
        StaticJsonDocument<80> doc;
        doc["action"] = "toggle";
        doc["relay"] = index;
        doc["state"] = state;
        String output;
        serializeJson(doc, output);
        ServerHandle->getWSocket()->textAll(output);
    }
}
