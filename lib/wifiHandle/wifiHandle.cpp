#include "wifiHandle.h"

void wifiHandle::init()
{
    _isWifiConnect = connectWifi(&WiFi);
    _isApConnect = connectAp(&WiFi);

    if (_isApConnect)
    {
        Serial.print("Ap was created name: ");
        Serial.println(ssidAp);
    }
    if (_isWifiConnect)
    {
        Serial.print("Wifi was connected to: ");
        Serial.println(ssid);
    }
    _TcpIpServer->begin();
}

bool wifiHandle::connectAp(WiFiClass *wifi)
{
    return WiFi.softAP(ssidAp, passwordAp);
}

bool wifiHandle::connectWifi(WiFiClass *wifi)
{
    return WiFi.begin(ssid, password);
}

void wifiHandle::onApConnect()
{
    _isApConnect = connectAp(&WiFi);
}

void wifiHandle::onWifiConnect()
{
    _isWifiConnect = connectWifi(&WiFi);
}

void wifiHandle::setWifiCrentials(String _ssid, String _password)
{
    ssid = _ssid;
    password = _password;
}

void wifiHandle::setApCrentials(String _ssid, String _password)
{
    ssidAp = _ssid;
    passwordAp = _password;
}