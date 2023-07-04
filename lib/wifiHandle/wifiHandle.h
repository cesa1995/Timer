#ifndef _WIFI_HANDLE_H
#define _WIFI_HANDLE_H
#include <WiFi.h>
class wifiHandle
{
private:
    String ssid = "LoadingWifi";
    String password = "AveoRojo123";
    String ssidAp = "Timer";
    String passwordAp = "";
    String deviceName = "Smart Leaf Timer";
    bool _isWifiConnect;
    bool _isApConnect;
    bool connectAp(WiFiClass *wifi);
    bool connectWifi(WiFiClass *wifi);

public:
    void init();
    void onWifiConnect();
    void onApConnect();
    void setWifiCrentials(String _ssid, String _password);
    void setApCrentials(String _ssid, String _password);

    IPAddress getWifiIp()
    {
        return WiFi.localIP();
    }

    IPAddress getApIp()
    {
        return WiFi.softAPIP();
    }

    String getSsidAp()
    {
        return ssidAp;
    }

    String getPasswordAp()
    {
        return passwordAp;
    }

    String getSsidWifi()
    {
        return ssid;
    }

    String getPasswordWifi()
    {
        return password;
    }

    bool isApConnect()
    {
        return _isApConnect;
    }

    bool isWifiConnect()
    {
        return WiFi.isConnected();
    }

    void setName(String _deviceName)
    {
        deviceName = _deviceName;
    }

    String getName()
    {
        return deviceName;
    }

    static void HandleWifiConnect(void *pvParameters)
    {
        wifiHandle *WifiHandle = (wifiHandle *)pvParameters;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        WifiHandle->onApConnect();
        WifiHandle->onWifiConnect();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
};
#endif