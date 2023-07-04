#ifndef _SERVER_HANDLE_H
#define _SERVER_HANDLE_H
#include <DNSServer.h>
#include "FFat.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include "memory.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "rtc.h"
#include "wifiHandle.h"
#include "relay.h"
#include <Update.h>
#define DEST_FS_USES_FFAT
#include <ESP32-targz.h>
class serverHandle
{
private:
    // Pages
    const char *index = "/index/";
    const char *relay = "/relay/";
    const char *ap = "/ap/";
    const char *wifi = "/wifi/";
    // static Files
    const char *config = "/config.json";
    const char *publicFiles = "/www/";
    // endpoints
    const char *writeConfig = "/config";
    const char *getData = "/getData";
    const char *upload = "/upload";
    const char *firmwareFile = "/firmware.bin";

    int isRelayPress = 5;
    bool shouldReboot = false;

    relays *Relays;
    memory *Memory;
    RTC *rtc;
    wifiHandle *WifiHandle;
    DNSServer dnsServer;
    AsyncWebServer *_server;
    AsyncWebSocket *_ws;

    // async task
    TaskHandle_t *WifiTask;
    TaskHandle_t *UpdateTask;

    String filename;

    void reponse(AsyncWebServerRequest *request, const char *file);
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, AsyncWebSocket *_ws);

public:
    serverHandle(memory *_memory, RTC *_rtc, wifiHandle *_wifiHandle, relays *_relays);
    void init();
    void loop();
    void handleDataRequest(JsonObject body);
    void handleJsonRequest(AsyncWebServerRequest *request, JsonVariant &json);
    void handleJsonGetData(AsyncWebServerRequest *request);

    void resetRelayPress()
    {
        isRelayPress = 5;
    }
    int getRelayPress()
    {
        return isRelayPress;
    }
    String IpAddressToString(const IPAddress &ipAddress);
    AsyncWebSocket *getWSocket()
    {
        return _ws;
    }

    String getFileName()
    {
        return filename;
    }

    void setFileName(String _filename)
    {
        filename = _filename;
    }

    static String processorConfigWifi(const String &var, serverHandle *ServerHandle);

    static void HandleUpdateTimer(void *pvParameters)
    {
        serverHandle *ServerHandle = (serverHandle *)pvParameters;
        Serial.println("Updating...!");
        Serial.printf("Free space: %10u\n", FFat.freeBytes());
        if (!tarGzFS.begin(4))
        {
            Serial.println("Could not start filesystem");
            while (1)
                ;
        }
        Serial.print("/" + ServerHandle->getFileName());
        File file = tarGzFS.open(ServerHandle->getFileName(), FILE_READ);
        if (!file)
        {
            Serial.println("Can't open file");
            vTaskDelay(100 / portTICK_PERIOD_MS);
            vTaskDelete(NULL);
        }

        Stream *streamptr = &file;

        TarGzUnpacker *TARGZUnpacker = new TarGzUnpacker();

        TARGZUnpacker->haltOnError(true);                                                            // stop on fail (manual restart/reset required)
        TARGZUnpacker->setTarVerify(true);                                                           // true = enables health checks but slows down the overall process
        TARGZUnpacker->setupFSCallbacks(targzTotalBytesFn, targzFreeBytesFn);                        // prevent the partition from exploding, recommended
        TARGZUnpacker->setGzProgressCallback(BaseUnpacker::defaultProgressCallback);                 // targzNullProgressCallback or defaultProgressCallback
        TARGZUnpacker->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);                    // gz log verbosity
        TARGZUnpacker->setTarProgressCallback(BaseUnpacker::defaultProgressCallback);                // prints the untarring progress for each individual file
        TARGZUnpacker->setTarStatusProgressCallback(BaseUnpacker::defaultTarStatusProgressCallback); // print the filenames as they're expanded
        TARGZUnpacker->setTarMessageCallback(BaseUnpacker::targzPrintLoggerCallback);                // tar log verbosity               // tar log verbosity

        if (!TARGZUnpacker->tarGzStreamExpander((Stream *)&file, tarGzFS))
        {
            Serial.printf("tarGzStreamExpander failed with return code #%d\n", TARGZUnpacker->tarGzGetError());
        }
        Serial.printf("Free space: %10u\n", FFat.freeBytes());
        file.close();

        ServerHandle->updateFromFS(FFat, ServerHandle->firmwareFile, &ServerHandle->shouldReboot);

        vTaskDelay(100 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }

    static void performUpdate(Stream &updateSource, size_t updateSize, bool *shouldReboot)
    {
        if (Update.begin(updateSize))
        {
            size_t written = Update.writeStream(updateSource);
            if (written == updateSize)
            {
                Serial.println("Written : " + String(written) + " successfully");
            }
            else
            {
                Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
            }
            if (Update.end())
            {
                Serial.println("OTA done!");
                if (Update.isFinished())
                {
                    Serial.println("Update successfully completed. Rebooting.");
                    *shouldReboot = true;
                }
                else
                {
                    Serial.println("Update not finished? Something went wrong!");
                }
            }
            else
            {
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            }
        }
        else
        {
            Serial.println("Not enough space to begin OTA");
        }
    }

    // check given FS for valid update.bin and perform update if available
    static void updateFromFS(fs::FS &fs, const char *firmwareFile, bool *shouldReboot)
    {
        File updateBin = fs.open(firmwareFile);
        if (updateBin)
        {
            if (updateBin.isDirectory())
            {
                Serial.println("Error, update.bin is not a file");
                updateBin.close();
                return;
            }

            size_t updateSize = updateBin.size();

            if (updateSize > 0)
            {
                Serial.println("Try to start update");
                performUpdate(updateBin, updateSize, shouldReboot);
            }
            else
            {
                Serial.println("Error, file is empty");
            }

            updateBin.close();

            // whe finished remove the binary from sd card to indicate end of the process
            fs.remove(firmwareFile);
        }
        else
        {
            Serial.println("Could not load firmware.bin from sd root");
        }
    }
};
#endif