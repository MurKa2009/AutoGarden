#include "main.h"

WebServer server(80);
Preferences preferences;
GyverHTU21D htu;

// Инициализация переменных состояния
float currentTemperature = 0;
float currentHumidity = 0;
int soilMoisture1 = 0;
int soilMoisture2 = 0;
int soilMoisture3 = 0;
bool lightDigitalValue = false;
bool lightState = false;
bool manualWindowControl = false;
bool manualWateringControl = false;
bool manualLightControl = false;
bool windowOpenState = false;
bool wateringActiveState = false;
bool screenNeedsUpdate = true;
unsigned long lastSensorUpdate = 0;
const unsigned long SENSOR_UPDATE_INTERVAL = 1000;

TaskHandle_t sensorTaskHandle;
TaskHandle_t displayTaskHandle;

// Состояния меню
//MenuState currentMenuState = MAIN_MENU;
//ControlState controlState = {false, false, false, false, false, false};

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

    initPins();
    initHTU21D();
    setupWiFi();
    loadSettings();
    setupScreen();

    if (isWiFiConnected()) {
        setupWebServer();
        Serial.print("Web server: http://");
        Serial.println(WiFi.localIP());
    }

    // Запуск задач на разных ядрах
    xTaskCreatePinnedToCore(
        sensorTask,      // Функция задачи
        "SensorTask",    // Имя задачи
        10000,          // Размер стека
        NULL,           // Параметры
        1,              // Приоритет
        &sensorTaskHandle, // Дескриптор задачи
        0               // Ядро (0 или 1)
    );

    xTaskCreatePinnedToCore(
        displayTask,     // Функция задачи
        "DisplayTask",  // Имя задачи
        10000,          // Размер стека
        NULL,           // Параметры
        1,              // Приоритет
        &displayTaskHandle, // Дескриптор задачи
        1               // Ядро (0 или 1)
    );

    Serial.println("System started");
}

void loop() {
    handleWiFiReconnect();
    
    if (isWiFiConnected()) {
        server.handleClient();
    }

    if (millis() - lastSensorUpdate > SENSOR_UPDATE_INTERVAL) {
        updateSensors();
        lastSensorUpdate = millis();
    }

    delay(1);
}

void initPins() {
    pinMode(RELAY_PIN_1, OUTPUT);
    pinMode(RELAY_PIN_2, OUTPUT);
    pinMode(RELAY_PIN_3, OUTPUT);
    pinMode(RELAY_PIN_4, OUTPUT);
    pinMode(WINDOW_SERVO_PIN, OUTPUT);
    pinMode(SOIL_MOISTURE_PIN_1, INPUT);
    pinMode(SOIL_MOISTURE_PIN_2, INPUT);
    pinMode(SOIL_MOISTURE_PIN_3, INPUT);
    pinMode(LIGHT_SENSOR_PIN, INPUT);

    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_3, LOW);
    digitalWrite(RELAY_PIN_4, LOW);
    digitalWrite(WINDOW_SERVO_PIN, LOW);
}

void initHTU21D() {
    Wire.begin(HTU21D_SDA, HTU21D_SCL);
    if (!htu.begin()) {
        Serial.println("HTU21D error!");
        while (1) delay(10);
    }
    Serial.println("HTU21D OK");
}

void updateSensors() {
    if (htu.readTick()) {
        currentTemperature = htu.getTemperature();
        currentHumidity = htu.getHumidity();
    }

    soilMoisture1 = analogRead(SOIL_MOISTURE_PIN_1);
    soilMoisture2 = analogRead(SOIL_MOISTURE_PIN_2);
    soilMoisture3 = analogRead(SOIL_MOISTURE_PIN_3);

    lightDigitalValue = digitalRead(LIGHT_SENSOR_PIN);
    if (!manualLightControl) {
        lightState = (lightDigitalValue == settings.lightThreshold);
        digitalWrite(RELAY_PIN_4, lightState ? HIGH : LOW);
    }

    static unsigned long lastLog = 0;
    if (millis() - lastLog > 5000) {
        Serial.printf("Temp: %.1fC, Hum: %.1f%%, Soil: %d/%d/%d, Light: %s\n",
                     currentTemperature, currentHumidity,
                     soilMoisture1, soilMoisture2, soilMoisture3,
                     lightDigitalValue ? "DARK" : "LIGHT");
        lastLog = millis();
    }
}

void manageGreenhouse() {
    static unsigned long lastControlTime = 0;
    if (millis() - lastControlTime > 2000) {
        // Управление поливом (только в автоматическом режиме)
        if (!manualWateringControl) {
            bool needWater1 = soilMoisture1 > settings.soilThreshold;
            bool needWater2 = soilMoisture2 > settings.soilThreshold;
            bool needWater3 = soilMoisture3 > settings.soilThreshold;
            
            digitalWrite(RELAY_PIN_1, needWater1 ? HIGH : LOW);
            digitalWrite(RELAY_PIN_2, needWater2 ? HIGH : LOW);
            digitalWrite(RELAY_PIN_3, needWater3 ? HIGH : LOW);
            wateringActiveState = needWater1 || needWater2 || needWater3;
        }
        
        // Управление освещением
        digitalWrite(RELAY_PIN_4, lightState ? HIGH : LOW);
        
        // Управление форточкой (только в автоматическом режиме)
        if (!manualWindowControl) {
            bool needVentilation = currentTemperature > settings.temperatureThreshold || 
                                  currentHumidity > settings.humidityThreshold;
            digitalWrite(WINDOW_SERVO_PIN, needVentilation ? HIGH : LOW);
            windowOpenState = needVentilation;
        }
        
        lastControlTime = millis();
    }
}

void sensorTask(void *pvParameters) {
    for(;;) {
        manageGreenhouse();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void displayTask(void *pvParameters) {
    for(;;) {
        handleEncoder();
        if (screenNeedsUpdate) {
            updateScreen();
            screenNeedsUpdate = false;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}