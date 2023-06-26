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
        return _isWifiConnect;
    }
};
#endif