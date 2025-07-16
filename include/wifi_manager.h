#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Preferences.h>

void setupWiFi();
bool isWiFiConnected();
void handleWiFiReconnect();
String getWiFiStatus();

#endif