#include <gtest/gtest.h>
#include "memory.h"
#include "wifiHandle.h"

class TextMemory : public testing::Test
{
private:
    memory *Memory;
    wifiHandle WifiHandle;

public:
    TextMemory()
    {
        Memory = new memory(&WifiHandle);
    }

    void SetUp() override
    {
        Memory->init();
        WifiHandle.init();
    }

    void testLoadDataAndConnectionWifi()
    {
        EXPECT_TRUE(WifiHandle.isApConnect());
        EXPECT_TRUE(WifiHandle.isWifiConnect());
    }

    void testAddTimer()
    {
        TimersConfigArray timers = Memory->getTimersByRelay(0);
        int currentLen = *timers.size;
        TimerConfig timer;
        timer.enableTimer = true;
        timer.endTime = 1000;
        timer.intervalStop = 1000;
        timer.intervalWork = 1000;
        timer.name = "New timer";
        timer.startTime = 1000;
        timer.cycle = timer.intervalWork + timer.intervalStop;
        timer.workcycle = timer.intervalWork / timer.cycle;
        timer.stopcycle = timer.intervalStop / timer.cycle;
        Memory->createTimerByRelay(0, timer);
        writeAndLoad();
        EXPECT_EQ(*timers.size, currentLen + 1);
    }

    void testDeleteTimer()
    {
        TimersConfigArray timers = Memory->getTimersByRelay(0);
        int currentLen = *timers.size;
        Memory->deleteTimerByRelay(0, *timers.size - 1);
        writeAndLoad();
        EXPECT_EQ(*timers.size, currentLen - 1);
    }

    void writeAndLoad()
    {
        Memory->writeDataJson();
        Memory->readDataConfig();
    }

    void testButtomConfig()
    {
        ButtomConfig currenConfig = Memory->getButtomConfigByRelay(0);
        ButtomConfig config;
        config.EndDelay = 3000;
        config.StartDelay = 1000;
        config.WorkDelay = 500;
        Memory->setButtomConfigByRelay(0, config);
        writeAndLoad();
        EXPECT_NE(Memory->getButtomConfigByRelay(0).EndDelay, currenConfig.EndDelay);
        EXPECT_NE(Memory->getButtomConfigByRelay(0).StartDelay, currenConfig.StartDelay);
        EXPECT_NE(Memory->getButtomConfigByRelay(0).WorkDelay, currenConfig.WorkDelay);
        Memory->setButtomConfigByRelay(0, currenConfig);
        Memory->writeDataJson();
    }
};

TEST_F(TextMemory, testConnectionWifiAndLoadData)
{
    testLoadDataAndConnectionWifi();
}

TEST_F(TextMemory, testAddAndDeleteTimerRelay0)
{
    testAddTimer();
    testDeleteTimer();
}

TEST_F(TextMemory, testChangeButtomConfigRelay0)
{
    testButtomConfig();
}