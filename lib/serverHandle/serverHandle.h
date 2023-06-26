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

    int isRelayPress = 5;

    relays *Relays;
    memory *Memory;
    RTC *rtc;
    wifiHandle *WifiHandle;
    DNSServer dnsServer;
    AsyncWebServer *_server;
    AsyncWebSocket *_ws;
    void reponse(AsyncWebServerRequest *request, const char *file);
    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, AsyncWebSocket *_ws);

public:
    serverHandle(memory *_memory, RTC *_rtc, wifiHandle *_wifiHandle, relays *_relays);
    void init();
    void loop();
    void handleDataRequest(JsonObject body);
    void handleJsonRequest(AsyncWebServerRequest *request, JsonVariant &json);
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
    static String processorConfigWifi(const String &var, serverHandle *ServerHandle);
};
#endif