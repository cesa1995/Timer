#include <Arduino.h>
#include <ArduinoJson.h>
#include <serverHandle.h>
#include <wifiHandle.h>
#include <memory.h>
#include <rtc.h>
#include <relay.h>
#include <buttom.h>
#include <timer.h>

TaskHandle_t NowTask;
TaskHandle_t ButtomTask;
TaskHandle_t TimerTask;

wifiHandle WifiHandle;
relays Relays;
RTC rtc;
memory Memory(&WifiHandle);
serverHandle ServerHandle(&Memory, &rtc, &WifiHandle, &Relays);
buttoms Buttoms(&Relays, &ServerHandle, &Memory, &rtc);
timer Timer(&Memory, &WifiHandle, &rtc, &Relays, &Buttoms);

void HandleNowTask(void *pvParameters)
{
  for (;;)
  {
    size_t client = ServerHandle.getWSocket()->count();
    if (client > (size_t)0)
    {
      StaticJsonDocument<80> doc;
      doc["action"] = "time";
      doc["now"] = rtc.timeToString();
      String output;
      serializeJson(doc, output);
      ServerHandle.getWSocket()->textAll(output);
      doc.clear();
    }
    vTaskDelay(30000 / portTICK_PERIOD_MS);
  }
}

void HandleButtomTask(void *pvParameters)
{
  for (;;)
  {
    Buttoms.checkButtoms();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void HandleTimerTask(void *pvParameters)
{
  for (;;)
  {
    Timer.timerCheckTime();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Memory.init();
  WifiHandle.init();
  rtc.init();
  ServerHandle.init();
  Buttoms.init();

  Serial.println("Testing OTA version!");

  xTaskCreatePinnedToCore(
      HandleNowTask,   /* Task function. */
      "HandleNowTask", /* name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      1,               /* priority of the task */
      &NowTask,        /* Task handle to keep track of created task */
      1);              /* pin task to core 0 */

  xTaskCreatePinnedToCore(
      HandleButtomTask,   /* Task function. */
      "HandleButtomTask", /* name of task. */
      10000,              /* Stack size of task */
      NULL,               /* parameter of the task */
      1,                  /* priority of the task */
      &ButtomTask,        /* Task handle to keep track of created task */
      1);

  xTaskCreatePinnedToCore(
      HandleTimerTask,   /* Task function. */
      "HandleTimerTask", /* name of task. */
      10000,             /* Stack size of task */
      NULL,              /* parameter of the task */
      20,                /* priority of the task */
      &TimerTask,        /* Task handle to keep track of created task */
      0);
  delay(500);
}

void loop()
{
  ServerHandle.loop();
}