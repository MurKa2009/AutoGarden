#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include <GyverHTU21D.h>
#include <ArduinoJson.h>
#include "wifi_manager.h"
#include "screen_manager.h"
#include "settings_manager.h"

// Пины
#define RELAY_PIN_1 4
#define RELAY_PIN_2 16
#define RELAY_PIN_3 17
#define RELAY_PIN_4 5
#define WINDOW_SERVO_PIN 18
#define SOIL_MOISTURE_PIN_1 32
#define SOIL_MOISTURE_PIN_2 33
#define SOIL_MOISTURE_PIN_3 34
#define LIGHT_SENSOR_PIN 35
#define HTU21D_SDA 21
#define HTU21D_SCL 22

extern WebServer server;
extern Preferences preferences;
extern GyverHTU21D htu;
extern Settings settings;

extern float currentTemperature;
extern float currentHumidity;
extern int soilMoisture1;
extern int soilMoisture2;
extern int soilMoisture3;
extern bool lightDigitalValue;
extern bool lightState;
extern bool manualWindowControl;
extern bool manualWateringControl;
extern bool manualLightControl;
extern bool windowOpenState;
extern bool wateringActiveState;
extern bool screenNeedsUpdate;
extern unsigned long lastSensorUpdate;
extern const unsigned long SENSOR_UPDATE_INTERVAL;

extern TaskHandle_t sensorTaskHandle;
extern TaskHandle_t displayTaskHandle;

void initPins();
void initHTU21D();
void updateSensors();
void manageGreenhouse();
void sensorTask(void *pvParameters);
void displayTask(void *pvParameters);
void setupWebServer();

#endif