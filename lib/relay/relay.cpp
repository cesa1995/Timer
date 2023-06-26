#include "relay.h"

void relay::init(byte pin)
{
    this->pin = pin;
    pinMode(pin, OUTPUT);
    sw_mode = LOW;
    off();
}

void relay::on()
{
    digitalWrite(pin, HIGH);
    state = HIGH;
}

void relay::off()
{
    digitalWrite(pin, LOW);
    state = LOW;
}

bool relay::toggle()
{
    if (state == HIGH)
    {
        digitalWrite(pin, LOW);
        state = LOW;
    }
    else if (state == LOW)
    {
        digitalWrite(pin, HIGH);
        state = HIGH;
    }
    return state;
}
void relay::rlswitch(byte swmode)
{
    if (swmode == 0)
    {
        off();
    }
    if (swmode == 1)
    {
        on();
    }
    if (swmode == 2)
    {
        toggle();
    }
}

relays::relays()
{
    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        data[i].init(pins[i]);
    }
}

relays::~relays()
{
}

void relays::setOnRelayByIndex(int index)
{
    data[index].on();
};
void relays::setOffRelayByIndex(int index)
{
    data[index].off();
};
bool relays::toggleRelayByIndex(int index)
{
    return data[index].toggle();
};
bool relays::getRelayStateByIndex(int index)
{
    return data[index].getState();
};
String relays::getRelayStateByIndexString(int index)
{
    return data[index].getStateString();
}