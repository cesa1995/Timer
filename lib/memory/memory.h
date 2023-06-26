#ifndef _MEMORY_H
#define _MEMORY_H
#include <wifiHandle.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FFat.h>

#define QUANTITYRELAYS 4

struct TimerConfig
{
    String name;
    int startTime;
    int endTime;
    int intervalWork;
    int intervalStop;
    double cycle;
    double workcycle;
    double stopcycle;
    bool enableTimer;
};

struct ButtomConfig
{
    int StartDelay;
    int WorkDelay;
    int EndDelay;
};

class Data
{
private:
    ButtomConfig buttomConfig;
    TimerConfig *config;
    int size;

public:
    Data()
    {
        cleanData();
    }
    ~Data()
    {
        delete[] config;
    }
    void enableTimer(bool enable, int index);
    void putTimer(TimerConfig timerConfig);
    void deleteTimerByIndex(int index);
    void setButtomConfig(ButtomConfig _buttomConfig);
    void cleanData();

    int *getSize()
    {
        return &size;
    }
    TimerConfig *getAllTimers()
    {
        return config;
    }
    TimerConfig getTimerByIndex(int index);
    ButtomConfig getButtomConfig()
    {
        return buttomConfig;
    }
};

struct Config
{
    String lan;
    Data data[QUANTITYRELAYS];
};

struct TimersConfigArray
{
    TimerConfig *timers;
    int *size;
};

class memory
{
private:
    DynamicJsonDocument *doc;
    wifiHandle *_wifiHandle;
    Config config;
    const char *fileName = "/config.json";
    void onReadDataJson(JsonArray Data);
    void onReadButtomConfig(int index, JsonObject relayData);
    void onReadTimerConfig(int index, JsonObject relayData);
    void cleanData();

public:
    memory(wifiHandle *WifiHandle);
    void init();
    void readDataConfig();
    void writeDataJson();
    void setButtomConfigByRelay(int relay, ButtomConfig buttomConfig);
    void deleteTimerByRelay(int relay, int index);
    void createTimerByRelay(int relay, TimerConfig timer);
    void enableTimer(int relay, int index, bool enable);
    void setLan(String _lan);
    String getLan()
    {
        return config.lan;
    }

    ~memory()
    {
        delete doc;
    }

    ButtomConfig getButtomConfigByRelay(int index)
    {
        if (index > QUANTITYRELAYS)
        {
            ButtomConfig config;
            config.EndDelay = 0;
            config.StartDelay = 0;
            config.WorkDelay = 0;
            return config;
        }
        return config.data[index].getButtomConfig();
    }
    TimersConfigArray getTimersByRelay(int index)
    {
        TimersConfigArray configArray;
        if (index + 1 > QUANTITYRELAYS)
        {
            configArray.size = 0;
            configArray.timers = new TimerConfig[0];
            return configArray;
        }
        configArray.size = config.data[index].getSize();
        configArray.timers = config.data[index].getAllTimers();
        return configArray;
    }
};

#endif
