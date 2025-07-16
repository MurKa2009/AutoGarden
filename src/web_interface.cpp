#include "main.h"
#include "web_interface.h"

void updateSoilMoisturePlaceholders(String& html, const String& num, int value) {
    String prefix = "SOIL" + num;
    html.replace("%" + prefix + "_VALUE%", String(value));
    
    // Определяем цвет в зависимости от порога
    String color = (value > settings.soilThreshold) ? "danger" : "success";
    html.replace("%" + prefix + "_COLOR%", color);
    
    // Прогресс-бар (инвертированный, так как меньше значение - больше влажность)
    int progress = min(100, (4095 - value) / (4095 - settings.soilThreshold) * 100);
    html.replace("%" + prefix + "_PROGRESS%", String(progress));
}

void setupWebServer() {
    server.on("/", HTTP_GET, []() {
        String html = MAIN_page;
        
        // Заменяем общие параметры
        html.replace("%IP%", WiFi.localIP().toString());
        html.replace("%WIFI_STATUS%", WiFi.status() == WL_CONNECTED ? "success" : "danger");
        html.replace("%WIFI_STRENGTH%", String(WiFi.RSSI()) + " dBm");
        
        // Показания датчиков
        html.replace("%TEMPERATURE%", String(currentTemperature, 1));
        html.replace("%HUMIDITY%", String(currentHumidity, 1));
        html.replace("%TEMP_PROGRESS%", String(currentTemperature/50*100));
        html.replace("%HUM_PROGRESS%", String(currentHumidity));
        html.replace("%LIGHT_VALUE%", lightDigitalValue ? "dark" : "warning");
        html.replace("%LIGHT_VALUE_TEXT%", lightDigitalValue ? "ТЕМНО" : "СВЕТЛО");
        html.replace("%LIGHT_MODE%", settings.lightThreshold ? "dark" : "light text-dark");
        html.replace("%LIGHT_MODE_TEXT%", settings.lightThreshold ? "Ночной" : "Дневной");
        
        // Влажность почвы
        updateSoilMoisturePlaceholders(html, "1", soilMoisture1);
        updateSoilMoisturePlaceholders(html, "2", soilMoisture2);
        updateSoilMoisturePlaceholders(html, "3", soilMoisture3);
        
        // Режимы автоматического управления
        html.replace("%TEMP_AUTO_MODE%", manualWindowControl ? "warning" : "success");
        html.replace("%TEMP_AUTO_TEXT%", manualWindowControl ? "Температура: Ручной" : "Температура: Авто");
        html.replace("%HUM_AUTO_MODE%", manualWindowControl ? "warning" : "success");
        html.replace("%HUM_AUTO_TEXT%", manualWindowControl ? "Влажность: Ручной" : "Влажность: Авто");
        html.replace("%LIGHT_AUTO_MODE%", manualLightControl ? "warning" : "success");
        html.replace("%LIGHT_AUTO_TEXT%", manualLightControl ? "Освещение: Ручной" : "Освещение: Авто");
        
        // Управление поливом
        html.replace("%WATER_AUTO%", manualWateringControl ? "outline-primary" : "primary");
        html.replace("%WATER_MANUAL%", manualWateringControl ? "primary" : "outline-primary");
        html.replace("%WATER_CONTROLS_DISPLAY%", manualWateringControl ? "block" : "none");
        
        // Состояние помп
        html.replace("%PUMP1_ON%", digitalRead(RELAY_PIN_1) ? "primary" : "outline-primary");
        html.replace("%PUMP1_OFF%", !digitalRead(RELAY_PIN_1) ? "danger" : "outline-danger");
        html.replace("%PUMP2_ON%", digitalRead(RELAY_PIN_2) ? "primary" : "outline-primary");
        html.replace("%PUMP2_OFF%", !digitalRead(RELAY_PIN_2) ? "danger" : "outline-danger");
        html.replace("%PUMP3_ON%", digitalRead(RELAY_PIN_3) ? "primary" : "outline-primary");
        html.replace("%PUMP3_OFF%", !digitalRead(RELAY_PIN_3) ? "danger" : "outline-danger");
        
        // Управление светом
        html.replace("%LIGHT_AUTO%", manualLightControl ? "outline-primary" : "primary");
        html.replace("%LIGHT_MANUAL%", manualLightControl ? "primary" : "outline-primary");
        html.replace("%LIGHT_CONTROLS_DISPLAY%", manualLightControl ? "block" : "none");
        html.replace("%LIGHT_ON%", lightState ? "warning" : "outline-warning");
        html.replace("%LIGHT_OFF%", !lightState ? "secondary" : "outline-secondary");
        
        // Управление форточкой
        html.replace("%WINDOW_AUTO%", manualWindowControl ? "outline-primary" : "primary");
        html.replace("%WINDOW_MANUAL%", manualWindowControl ? "primary" : "outline-primary");
        html.replace("%WINDOW_CONTROLS_DISPLAY%", manualWindowControl ? "block" : "none");
        html.replace("%WINDOW_OPEN%", windowOpenState ? "success" : "outline-success");
        html.replace("%WINDOW_CLOSE%", !windowOpenState ? "danger" : "outline-danger");
        
        // Настройки
        html.replace("%TEMP_THRESHOLD%", String(settings.temperatureThreshold, 1));
        html.replace("%HUM_THRESHOLD%", String(settings.humidityThreshold, 1));
        html.replace("%SOIL_THRESHOLD%", String(settings.soilThreshold));
        html.replace("%LIGHT_MODE_0%", settings.lightThreshold == 0 ? "selected" : "");
        html.replace("%LIGHT_MODE_1%", settings.lightThreshold == 1 ? "selected" : "");
        
        server.send(200, "text/html", html);
    });

    server.on("/data", HTTP_GET, []() {
        JsonDocument doc;
        doc["temperature"] = currentTemperature;
        doc["humidity"] = currentHumidity;
        doc["soil1"] = soilMoisture1;
        doc["soil2"] = soilMoisture2;
        doc["soil3"] = soilMoisture3;
        doc["soilThreshold"] = settings.soilThreshold;
        doc["lightValue"] = lightDigitalValue;
        doc["lightMode"] = settings.lightThreshold;
        doc["manualWindow"] = manualWindowControl;
        doc["manualWater"] = manualWateringControl;
        doc["manualLight"] = manualLightControl;
        doc["windowState"] = windowOpenState;
        doc["pump1State"] = digitalRead(RELAY_PIN_1);
        doc["pump2State"] = digitalRead(RELAY_PIN_2);
        doc["pump3State"] = digitalRead(RELAY_PIN_3);
        doc["lightState"] = lightState;
        doc["autoTemp"] = !manualWindowControl;
        doc["autoHum"] = !manualWindowControl;

        String json;
        serializeJson(doc, json);
        server.send(200, "application/json", json);
    });

    server.on("/control", HTTP_GET, []() {
        String type = server.arg("type");
        String action = server.arg("action");
        
        if (type.startsWith("pump")) {
            // Управление конкретной помпой (pump1, pump2, pump3)
            int pumpNum = type.charAt(4) - '0';
            if (pumpNum >= 1 && pumpNum <= 3) {
                int pin = pumpNum == 1 ? RELAY_PIN_1 : (pumpNum == 2 ? RELAY_PIN_2 : RELAY_PIN_3);
                if (action == "on") {
                    digitalWrite(pin, HIGH);
                } else if (action == "off") {
                    digitalWrite(pin, LOW);
                }
            }
        }
        else if (type == "window") {
            if (action == "open") {
                windowOpenState = true;
                digitalWrite(WINDOW_SERVO_PIN, HIGH);
            }
            else if (action == "close") {
                windowOpenState = false;
                digitalWrite(WINDOW_SERVO_PIN, LOW);
            }
            else if (action == "auto") {
                manualWindowControl = false;
            }
            else if (action == "manual") {
                manualWindowControl = true;
            }
        }
        else if (type == "water") {
            if (action == "auto") {
                manualWateringControl = false;
            }
            else if (action == "manual") {
                manualWateringControl = true;
            }
        }
        else if (type == "light") {
            if (action == "on") {
                lightState = true;
                digitalWrite(RELAY_PIN_4, HIGH);
            }
            else if (action == "off") {
                lightState = false;
                digitalWrite(RELAY_PIN_4, LOW);
            }
            else if (action == "auto") {
                manualLightControl = false;
            }
            else if (action == "manual") {
                manualLightControl = true;
            }
        }
        
        server.send(200, "application/json", "{\"success\":true}");
    });

    server.on("/settings", HTTP_POST, []() {
        if (server.hasArg("plain")) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, server.arg("plain"));
            
            if (error) {
                server.send(400, "text/plain", "Bad Request");
                return;
            }
            
            settings.temperatureThreshold = doc["temp"];
            settings.humidityThreshold = doc["hum"];
            settings.soilThreshold = doc["soil"];
            settings.lightThreshold = doc["light"];
            saveSettings();
            
            server.send(200, "application/json", "{\"success\":true}");
        } else {
            server.send(400, "text/plain", "Bad Request");
        }
    });
    
    server.begin();
}