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
    WiFiServer *_TcpIpServer = new WiFiServer(23);
    WiFiClient RemoteClient;

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

    void setName(String _deviceName)
    {
        deviceName = _deviceName;
    }

    String getName()
    {
        return deviceName;
    }

    void CheckTcpConnections()
    {
        if (_TcpIpServer->hasClient())
        {
            if (RemoteClient.connected())
            {
                Serial.println("Connection rejected");
                _TcpIpServer->available().stop();
            }
            else
            {
                Serial.println("Connection accepted");
                RemoteClient = _TcpIpServer->available();
            }
            SendDeviceId();
        }
    }

    void SendDeviceId()
    {
        if (RemoteClient.connected())
        {
            RemoteClient.println(deviceName);
        }
    }
};
#endif