#include "wifi_manager.h"

const char* ssid = "HOME-111";
const char* password = "06061986";
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 2000; // 30 секунд

void setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    Serial.println();
    Serial.print("Подключение к ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi подключён");
        Serial.print("IP адрес: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("Не удалось подключиться к WiFi!");
    }
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void handleWiFiReconnect() {
    if (!isWiFiConnected() && millis() - lastReconnectAttempt > RECONNECT_INTERVAL) {
        Serial.println("Попытка переподключения к WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
        lastReconnectAttempt = millis();
    }
}

String getWiFiStatus() {
    switch(WiFi.status()) {
        case WL_CONNECTED: return "Подключён";
        case WL_NO_SSID_AVAIL: return "Сеть не найдена";
        case WL_CONNECT_FAILED: return "Ошибка подключения";
        case WL_IDLE_STATUS: return "Ожидание";
        case WL_DISCONNECTED: return "Отключён";
        default: return "Неизвестный статус";
    }
}