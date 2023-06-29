#include "serverHandle.h"

serverHandle::serverHandle(memory *_memory, RTC *_rtc, wifiHandle *_wifiHandle, relays *_relays)
{
    _server = NULL;
    Memory = _memory;
    rtc = _rtc;
    WifiHandle = _wifiHandle;
    Relays = _relays;
}

void serverHandle::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, AsyncWebSocket *_ws)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            data[len] = 0;
            StaticJsonDocument<80> doc;
            DeserializationError error = deserializeJson(doc, (char *)data);
            if (error)
            {
                Serial.print("deserializeJson() failed in Ws: ");
                Serial.println(error.c_str());
                return;
            }
            int relay = doc["relay"]; // 1
            String action = doc["action"] | "";
            if (action == "toggle")
            {
                isRelayPress = relay;
            }
            doc.clear();
        }
        break;
    }
}

void serverHandle::init()
{
    _server = new AsyncWebServer(80);
    _ws = new AsyncWebSocket("/ws");

    dnsServer.start(53, "timer.local", WiFi.softAPIP());
    if (!MDNS.begin("timer"))
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                 { this->onEvent(server, client, type, arg, data, len, this->getWSocket()); });

    _server->addHandler(_ws);

    _server->on("/", [this](AsyncWebServerRequest *request)
                { this->reponse(request, index); });
    _server->on(relay, [this](AsyncWebServerRequest *request)
                { this->reponse(request, relay); });
    _server->on(ap, [this](AsyncWebServerRequest *request)
                { this->reponse(request, ap); });
    _server->on(wifi, [this](AsyncWebServerRequest *request)
                { this->reponse(request, wifi); });

    AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler(writeConfig, [this](AsyncWebServerRequest *request, JsonVariant &json)
                                                                           { this->handleJsonRequest(request, json); });
    _server->addHandler(handler);

    _server->on(config, HTTP_GET, [this](AsyncWebServerRequest *request)
                { request->send(FFat, config, String(), false); });

    _server->serveStatic(publicFiles, FFat, publicFiles);
    _server->begin();
}

void serverHandle::handleJsonRequest(AsyncWebServerRequest *request, JsonVariant &json)
{
    JsonObject jsonObj = json.as<JsonObject>();
    handleDataRequest(jsonObj);
    AsyncJsonResponse *response = new AsyncJsonResponse();
    response->addHeader("Server", "ESP Async Web Server");
    JsonObject root = response->getRoot();
    root["data"] = jsonObj;
    root["url"] = "http://timer.local";
    response->setLength();
    request->send(response);
}

void serverHandle::reponse(AsyncWebServerRequest *request, const char *file)
{
    String initUrl = file + Memory->getLan() + "/index.html";
    request->send(FFat, initUrl, String(), false, [this](const String &var)
                  { return this->processorConfigWifi(var, this); });
}

void serverHandle::loop()
{
    _ws->cleanupClients();
    dnsServer.processNextRequest();
}

void serverHandle::handleDataRequest(JsonObject body)
{
    if (body.containsKey("deviceName"))
    {
        WifiHandle->setName(body["deviceName"]);
        Memory->writeDataJson();
        return;
    }
    if (body.containsKey("lan"))
    {
        Memory->setLan(body["lan"]);
        Memory->writeDataJson();
        return;
    }
    if (body.containsKey("name"))
    {
        TimerConfig timer;
        timer.name = body["name"] | "";
        timer.endTime = body["end"] | 0;
        timer.startTime = body["start"] | 0;
        timer.intervalStop = body["intervalEnd"] | 0;
        timer.intervalWork = body["intervalStart"] | 0;
        timer.enableTimer = true;
        timer.cycle = timer.intervalWork + timer.intervalStop;
        timer.workcycle = timer.intervalWork / timer.cycle;
        timer.stopcycle = timer.intervalStop / timer.cycle;

        int relay = body["relay"] | 0;
        Memory->createTimerByRelay(relay, timer);
        Memory->writeDataJson();
        return;
    }
    if (body.containsKey("delayEnd"))
    {
        ButtomConfig buttomConfig;
        buttomConfig.EndDelay = body["delayEnd"] | 0;
        buttomConfig.StartDelay = body["delayStart"] | 0;
        buttomConfig.WorkDelay = body["delayWork"] | 0;
        int relay = body["relay"] | 0;
        Memory->setButtomConfigByRelay(relay, buttomConfig);
        Memory->writeDataJson();
        return;
    }
    if (body.containsKey("checked"))
    {
        int relay = body["relay"] | 0;
        int index = body["index"] | 0;
        bool checked = body["checked"] | false;
        Memory->enableTimer(relay, index, checked);
        Memory->writeDataJson();
        return;
    }
    if (body.containsKey("index"))
    {
        int relay = body["relay"] | 0;
        int index = body["index"] | 0;
        Memory->deleteTimerByRelay(relay, index);
        Memory->writeDataJson();
        return;
    }
    if (body.containsKey("ssidAp"))
    {
        String ssid = body["ssidAp"] | "Esp32";
        String password = body["passwordAp"] | "";
        WifiHandle->setApCrentials(ssid, password);
        Memory->writeDataJson();
        WifiHandle->onApConnect();
        return;
    }
    if (body.containsKey("ssid"))
    {
        String ssid = body["ssid"] | "";
        String password = body["password"] | "";
        WifiHandle->setWifiCrentials(ssid, password);
        Memory->writeDataJson();
        WifiHandle->onWifiConnect();
        return;
    }
    if (body.containsKey("now"))
    {
        int now = body["now"] | 1686801600;
        rtc->configDateTime(now);
        return;
    }
}

String serverHandle::IpAddressToString(const IPAddress &ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

String serverHandle::processorConfigWifi(const String &var, serverHandle *ServerHandle)
{
    if (var == "currentTime")
    {
        return ServerHandle->rtc->timeToString();
    }
    if (var == "WIFIIP")
    {
        return ServerHandle->IpAddressToString(WiFi.localIP());
    }
    if (var == "state1")
    {
        return ServerHandle->Relays->getRelayStateByIndexString(0);
    }
    if (var == "state2")
    {
        return ServerHandle->Relays->getRelayStateByIndexString(1);
    }
    if (var == "state3")
    {
        return ServerHandle->Relays->getRelayStateByIndexString(2);
    }
    if (var == "state4")
    {
        return ServerHandle->Relays->getRelayStateByIndexString(3);
    }
    return "";
}
