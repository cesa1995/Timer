#include "memory.h"

// memory
void memory::init()
{
    if (!FFat.begin())
    {
        FFat.format();
        Serial.println("FFat Mount Failed");
        return;
    }
    readDataConfig();
}

void memory::cleanData()
{
    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        config.data[i].cleanData();
    }
}

void memory::enableTimer(int relay, int index, bool enable)
{
    config.data[relay].enableTimer(enable, index);
}

void memory::setLan(String _lan)
{
    config.lan = _lan;
}

void memory::deleteTimerByRelay(int relay, int index)
{
    if (relay > QUANTITYRELAYS - 1)
    {
        return;
    }
    config.data[relay].deleteTimerByIndex(index);
}

void memory::setButtomConfigByRelay(int relay, ButtomConfig buttomConfig)
{
    if (relay > QUANTITYRELAYS - 1)
    {
        return;
    }
    config.data[relay].setButtomConfig(buttomConfig);
}

void memory::createTimerByRelay(int relay, TimerConfig timer)
{
    if (relay > QUANTITYRELAYS - 1)
    {
        return;
    }
    config.data[relay].putTimer(timer);
}

void memory::writeDataJson()
{
    doc = new DynamicJsonDocument(ESP.getMaxAllocHeap());
    JsonObject root = doc->to<JsonObject>();
    root["lan"] = config.lan;
    root["deviceName"] = _wifiHandle->getName();

    JsonObject credentialAp = root.createNestedObject("credentialsAp");
    credentialAp["ssid"] = _wifiHandle->getSsidAp();
    credentialAp["password"] = _wifiHandle->getPasswordAp();

    JsonObject credentialWifi = root.createNestedObject("credentialsWifi");
    credentialWifi["ssid"] = _wifiHandle->getSsidWifi();
    credentialWifi["password"] = _wifiHandle->getPasswordWifi();

    JsonArray Data = root.createNestedArray("Data");
    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        ButtomConfig buttomConfig = config.data[i].getButtomConfig();
        JsonObject relay = Data.createNestedObject();
        JsonObject buttom = relay.createNestedObject("buttom");
        buttom["Manual Start Delay"] = buttomConfig.StartDelay;
        buttom["Manual Work Delay"] = buttomConfig.WorkDelay;
        buttom["Manual End Delay"] = buttomConfig.EndDelay;

        JsonArray timers = relay.createNestedArray("timer");
        TimersConfigArray timersData = getTimersByRelay(i);
        for (int i = 0; i < *timersData.size; i++)
        {
            JsonObject timer = timers.createNestedObject();
            timer["Name"] = timersData.timers[i].name;
            timer["Start Time"] = timersData.timers[i].startTime;
            timer["End Time"] = timersData.timers[i].endTime;
            timer["Interval Start Time"] = timersData.timers[i].intervalWork;
            timer["Interval End Time"] = timersData.timers[i].intervalStop;
            timer["Enable Timer"] = timersData.timers[i].enableTimer;
        }
    }

    File configFile = FFat.open(fileName, FILE_WRITE);
    // serializeJsonPretty(root, Serial);
    if (serializeJson(root, configFile) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }
    configFile.close();
    delete doc;
}

void memory::readDataConfig()
{
    cleanData();
    File configFile = FFat.open(fileName);
    doc = new DynamicJsonDocument(ESP.getMaxAllocHeap());
    DeserializationError error = deserializeJson(*doc, configFile);
    JsonObject root = doc->as<JsonObject>();
    // serializeJsonPretty(root, Serial);
    if (error)
    {
        Serial.println(F("Failed to read file, using default configuration"));
        configFile.close();
        writeDataJson();
    }
    _wifiHandle->setName(root["deviceName"] | "");
    _wifiHandle->setWifiCrentials(root["credentialsWifi"]["ssid"] | "", root["credentialsWifi"]["password"] | "");
    _wifiHandle->setApCrentials(root["credentialsAp"]["ssid"] | "Esp32", root["credentialsAp"]["password"] | "");
    config.lan = root["lan"] | "";

    onReadDataJson(root["Data"]);
    configFile.close();

    delete doc;
}

void memory::onReadDataJson(JsonArray Data)
{
    for (int i = 0; i < QUANTITYRELAYS; i++)
    {
        JsonObject relayData = Data[i];
        onReadButtomConfig(i, relayData);
        onReadTimerConfig(i, relayData);
    }
}

void memory::onReadTimerConfig(int index, JsonObject relayData)
{
    JsonArray timerData = relayData["timer"];
    for (int i = 0; i < timerData.size(); i++)
    {
        JsonObject timer = timerData[i];
        TimerConfig timerConfig;
        timerConfig.name = timer["Name"] | "";
        timerConfig.startTime = timer["Start Time"] | 0;
        timerConfig.endTime = timer["End Time"] | 0;
        timerConfig.intervalWork = timer["Interval Start Time"] | 0;
        timerConfig.intervalStop = timer["Interval End Time"] | 0;
        timerConfig.enableTimer = timer["Enable Timer"] | false;
        timerConfig.cycle = timerConfig.intervalWork + timerConfig.intervalStop;
        timerConfig.workcycle = timerConfig.intervalWork / timerConfig.cycle;
        timerConfig.stopcycle = timerConfig.intervalStop / timerConfig.cycle;

        config.data[index].putTimer(timerConfig);
    }
}

void memory::onReadButtomConfig(int index, JsonObject relayData)
{
    JsonObject buttom = relayData["buttom"];
    ButtomConfig buttomConfig;
    buttomConfig.StartDelay = buttom["Manual Start Delay"];
    buttomConfig.WorkDelay = buttom["Manual Work Delay"];
    buttomConfig.EndDelay = buttom["Manual End Delay"];
    config.data[index].setButtomConfig(buttomConfig);
}

memory::memory(wifiHandle *WifiHandle)
{
    _wifiHandle = WifiHandle;
}

// Data
void Data::cleanData()
{
    size = 0;
    config = new TimerConfig[size];
    buttomConfig.EndDelay = 0;
    buttomConfig.WorkDelay = 0;
    buttomConfig.StartDelay = 0;
}

void Data::setButtomConfig(ButtomConfig _buttomConfig)
{
    buttomConfig = _buttomConfig;
}

void Data::enableTimer(bool enable, int index)
{
    config[index].enableTimer = enable;
}

void Data::putTimer(TimerConfig timerConfig)
{
    TimerConfig *temp = new TimerConfig[size + 1];
    for (int i = 0; i < size; i++)
    {
        temp[i] = config[i];
    }
    delete[] config;
    config = temp;
    config[size] = timerConfig;
    size++;
}

void Data::deleteTimerByIndex(int index)
{
    if (size < index + 1)
    {
        return;
    }
    TimerConfig *temp = new TimerConfig[size - 1];
    for (int i = 0; i < size; i++)
    {
        if (i < index)
        {
            temp[i] = config[i];
        }
        if (i > index)
        {
            temp[i - 1] = config[i];
        }
    }
    delete[] config;
    config = temp;
    size--;
}

TimerConfig Data::getTimerByIndex(int index)
{
    if (size < index)
    {
        TimerConfig empty;
        empty.name = "";
        empty.enableTimer = false;
        empty.endTime = 0;
        empty.intervalStop = 0;
        empty.intervalWork = 0;
        empty.startTime = 0;
        return empty;
    }

    return config[index];
}
