#include "screen_manager.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "FontsRus/TimesNRCyr6.h"
#include "main.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_BUTTON);

// Состояния системы
ScreenState currentScreen = MAIN_MENU;
SettingsSubState settingsSubState = SETTINGS_MAIN;

// Переменные меню
int menuItem = 0;
int settingsItem = 0;
int controlItem = 0;
int pumpControlItem = 0;
int lightControlItem = 0;
int windowControlItem = 0;
int deviceState = 0;

// Управление устройствами
bool isLightManual = false;

// Константы меню
const int menuItemsCount = 6;
const char* menuItems[] = {
    "Почва",
    "Температура",
    "Влажность",
    "Свет",
    "Управление",
    "Настройки"
};

const char* settingsItems[] = {
    "Температура",
    "Влажность",
    "Почва",
    "Свет",
    "Назад"
};

const char* controlItems[] = {
    "Полив",
    "Свет",
    "Форточка",
    "Назад"
};

const char* pumpItems[] = {
    "Помпа 1",
    "Помпа 2",
    "Помпа 3",
    "Режим",
    "Назад"
};

// Таймеры
unsigned long lastEncoderTurnTime = 0;
#define AUTOSCROLL_DELAY 500
#define AUTOSCROLL_INTERVAL 200
unsigned long autoScrollStartTime = 0;
bool autoScrollActive = false;
int autoScrollDirection = 0;

void setupScreen() {
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setRotation(1);
    tft.setTextWrap(false);
    tft.setFont(&TimesNRCyr6pt8b);
    
    encoder.setType(TYPE2);
    encoder.setTickMode(MANUAL);
}

// ==================== УПРАВЛЕНИЕ УСТРОЙСТВАМИ ====================
void togglePump(int pumpNum) {
    switch(pumpNum) {
        case 0: digitalWrite(RELAY_PIN_1, !digitalRead(RELAY_PIN_1)); break;
        case 1: digitalWrite(RELAY_PIN_2, !digitalRead(RELAY_PIN_2)); break;
        case 2: digitalWrite(RELAY_PIN_3, !digitalRead(RELAY_PIN_3)); break;
        case 3: manualWateringControl = !manualWateringControl; break;
    }
    setScreenUpdateFlag(true);
}

void toggleLightState() {
    if (manualLightControl) {
        lightState = !lightState;
        digitalWrite(RELAY_PIN_4, lightState ? HIGH : LOW);
        setScreenUpdateFlag(true);
    }
}

void toggleLightMode() {
    manualLightControl = !manualLightControl;
    if (!manualLightControl) {
        lightState = (lightDigitalValue == settings.lightThreshold);
        digitalWrite(RELAY_PIN_4, lightState ? HIGH : LOW);
    }
    saveSettings();
    setScreenUpdateFlag(true);
}

void toggleWindow() {
    windowOpenState = !windowOpenState;
    digitalWrite(WINDOW_SERVO_PIN, windowOpenState ? HIGH : LOW);
    setScreenUpdateFlag(true);
}

void toggleWindowMode() {
    manualWindowControl = !manualWindowControl;
    setScreenUpdateFlag(true);
}

void adjustSetting(int direction) {
    switch(settingsSubState) {
        case SETTINGS_TEMP:
            settings.temperatureThreshold += direction * 0.5;
            if(settings.temperatureThreshold < 10) settings.temperatureThreshold = 10;
            if(settings.temperatureThreshold > 40) settings.temperatureThreshold = 40;
            break;
        case SETTINGS_HUM:
            settings.humidityThreshold += direction * 5;
            if(settings.humidityThreshold < 30) settings.humidityThreshold = 30;
            if(settings.humidityThreshold > 90) settings.humidityThreshold = 90;
            break;
        case SETTINGS_SOIL:
            settings.soilThreshold += direction * 100;
            if(settings.soilThreshold < 500) settings.soilThreshold = 500;
            if(settings.soilThreshold > 3000) settings.soilThreshold = 3000;
            break;
    }
    saveSettings();
    setScreenUpdateFlag(true);
}

// ==================== НАВИГАЦИЯ ПО МЕНЮ ====================
void navigateMenu(bool direction) {
    switch(currentScreen) {
        case MAIN_MENU:
            menuItem = (menuItem + (direction ? 1 : -1) + menuItemsCount) % menuItemsCount;
            break;
        case SETTINGS_MENU:
            if(settingsSubState == SETTINGS_MAIN) {
                settingsItem = (settingsItem + (direction ? 1 : -1) + 5) % 5;
            }
            break;
        case CONTROL_MENU:
            controlItem = (controlItem + (direction ? 1 : -1) + 4) % 4;
            break;
        case PUMP_CONTROL:
            pumpControlItem = (pumpControlItem + (direction ? 1 : -1) + 5) % 5;
            break;
        case LIGHT_CONTROL:
            lightControlItem = (lightControlItem + (direction ? 1 : -1) + 3) % 3;
            break;
        case WINDOW_CONTROL:
            windowControlItem = (windowControlItem + (direction ? 1 : -1) + 3) % 3;
            break;
    }
    setScreenUpdateFlag(true);
}

void selectMenuItem(int item) {
    switch(item) {
        case 0: currentScreen = SOIL_MENU; break;
        case 1: currentScreen = TEMP_MENU; break;
        case 2: currentScreen = HUMIDITY_MENU; break;
        case 3: currentScreen = LIGHT_MENU; break;
        case 4: currentScreen = CONTROL_MENU; break;
        case 5: currentScreen = SETTINGS_MENU; break;
    }
    setScreenUpdateFlag(true);
}

void handleEncoder() {
    encoder.tick();
    
    if (encoder.isTurn()) {
        unsigned long now = millis();
        if (now - lastEncoderTurnTime > ENCODER_DEBOUNCE_TIME) {
            lastEncoderTurnTime = now;
            
            bool direction = encoder.isRight();
            
            if (!autoScrollActive) {
                autoScrollStartTime = now;
                autoScrollActive = true;
                autoScrollDirection = direction ? 1 : -1;
            }
            
            navigateMenu(direction);
        }
    } else {
        autoScrollActive = false;
    }
    
    if (autoScrollActive && millis() - autoScrollStartTime > AUTOSCROLL_DELAY) {
        if (millis() - lastEncoderTurnTime > AUTOSCROLL_INTERVAL) {
            lastEncoderTurnTime = millis();
            navigateMenu(autoScrollDirection == 1);
        }
    }
    
    if (encoder.isClick()) {
        switch(currentScreen) {
            case MAIN_MENU:
                selectMenuItem(menuItem);
                break;
            case SETTINGS_MENU:
                if(settingsSubState == SETTINGS_MAIN) {
                    if(settingsItem == 4) {
                        currentScreen = MAIN_MENU;
                    } else {
                        settingsSubState = (SettingsSubState)(settingsItem + 1);
                    }
                } else {
                    settingsSubState = SETTINGS_MAIN;
                }
                break;
            case CONTROL_MENU:
                if(controlItem == 0) currentScreen = PUMP_CONTROL;
                else if(controlItem == 1) currentScreen = LIGHT_CONTROL;
                else if(controlItem == 2) currentScreen = WINDOW_CONTROL;
                else currentScreen = MAIN_MENU;
                break;
            case PUMP_CONTROL:
                if(pumpControlItem == 4) currentScreen = CONTROL_MENU;
                else togglePump(pumpControlItem);
                break;
            case LIGHT_CONTROL:
                if(lightControlItem == 2) currentScreen = CONTROL_MENU;
                else if(lightControlItem == 0) toggleLightState();
                else if(lightControlItem == 1) toggleLightMode();
                break;
            case WINDOW_CONTROL:
                if(windowControlItem == 2) currentScreen = CONTROL_MENU;
                else if(windowControlItem == 0) toggleWindow();
                else if(windowControlItem == 1) toggleWindowMode();
                break;
            case SOIL_MENU:
            case TEMP_MENU:
            case HUMIDITY_MENU:
            case LIGHT_MENU:
                currentScreen = MAIN_MENU;
                break;
            default:
                if (currentScreen >= SETTINGS_TEMP && currentScreen <= SETTINGS_LIGHT) {
                    // Обработка подменю настроек
                    settingsSubState = SETTINGS_MAIN;
                } else {
                    currentScreen = MAIN_MENU;
                }
        }
        setScreenUpdateFlag(true);
    }
}

// ==================== ОТРИСОВКА ЭКРАНОВ ====================
void drawMainMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ГЛАВНОЕ МЕНЮ ");
    
    int startY = 35;
    for(int i = 0; i < menuItemsCount; i++) {
        tft.setCursor(15, startY + i * 15);
        
        if(i == menuItem) {
            tft.setTextColor(ST77XX_WHITE);
            tft.print(">");
            tft.print(menuItems[i]);
            tft.drawFastHLine(15, startY + i * 15 + 5, strlen(menuItems[i]) * 12, ST77XX_WHITE);
        } else {
            tft.setTextColor(ST77XX_WHITE);
            tft.print(" ");
            tft.print(menuItems[i]);
        }
    }
}

void drawSettingsMenu() {
    tft.fillScreen(ST77XX_BLACK);
    
    if(settingsSubState == SETTINGS_MAIN) {
        tft.setCursor(10, 15);
        tft.setTextColor(ST77XX_GREEN);
        tft.println(" НАСТРОЙКИ ");
        
        int startY = 35;
        for(int i = 0; i < 5; i++) {
            tft.setCursor(15, startY + i * 15);
            
            if(i == settingsItem) {
                tft.setTextColor(ST77XX_WHITE);
                tft.print(">");
                tft.print(settingsItems[i]);
                if(i == 4) {
                    tft.drawFastHLine(15, startY + i * 15 + 5, strlen(settingsItems[i]) * 12, ST77XX_WHITE);
                }
            } else {
                tft.setTextColor(i == 4 ? ST77XX_YELLOW : ST77XX_WHITE);
                tft.print(" ");
                tft.print(settingsItems[i]);
            }
        }
    } else {
        switch(settingsSubState) {
            case SETTINGS_TEMP: drawSettingsTempMenu(); break;
            case SETTINGS_HUM: drawSettingsHumMenu(); break;
            case SETTINGS_SOIL: drawSettingsSoilMenu(); break;
            case SETTINGS_LIGHT: drawSettingsLightMenu(); break;
        }
    }
}

void drawSettingsTempMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ПОРОГ ТЕМПЕРАТУРЫ ");
    
    tft.setCursor(15, 35);
    tft.print("Текущее: ");
    tft.print(settings.temperatureThreshold, 1);
    tft.print(" C");
    
    tft.setCursor(15, 50);
    tft.print("Новое значение:");
    
    tft.setCursor(15, 65);
    tft.print("▲ ");
    tft.print(settings.temperatureThreshold + 0.5, 1);
    tft.print(" C ▼");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawSettingsHumMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ПОРОГ ВЛАЖНОСТИ ");
    
    tft.setCursor(15, 35);
    tft.print("Текущее: ");
    tft.print(settings.humidityThreshold, 0);
    tft.print(" %");
    
    tft.setCursor(15, 50);
    tft.print("Новое значение:");
    
    tft.setCursor(15, 65);
    tft.print("▲ ");
    tft.print(settings.humidityThreshold + 5, 0);
    tft.print(" % ▼");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawSettingsSoilMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ПОРОГ ПОЧВЫ ");
    
    tft.setCursor(15, 35);
    tft.print("Текущее: ");
    tft.print(settings.soilThreshold);
    
    tft.setCursor(15, 50);
    tft.print("Новое значение:");
    
    tft.setCursor(15, 65);
    tft.print("▲ ");
    tft.print(settings.soilThreshold + 100);
    tft.print(" ▼");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawSettingsLightMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" РЕЖИМ СВЕТА ");
    
    tft.setCursor(15, 35);
    tft.print("Текущий: ");
    tft.print(settings.lightThreshold ? "Ночной" : "Дневной");
    
    tft.setCursor(15, 50);
    tft.print("Новый режим:");
    
    tft.setCursor(15, 65);
    tft.print("> ");
    tft.print(settings.lightThreshold ? "Дневной" : "Ночной");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawControlMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" УПРАВЛЕНИЕ ");
    
    int startY = 35;
    for(int i = 0; i < 4; i++) {
        tft.setCursor(15, startY + i * 15);
        
        if(i == controlItem) {
            tft.setTextColor(ST77XX_WHITE);
            tft.print(">");
            tft.print(controlItems[i]);
            if(i < 3) {
                tft.drawFastHLine(15, startY + i * 15 + 5, strlen(controlItems[i]) * 12, ST77XX_WHITE);
            }
        } else {
            tft.setTextColor(i == 3 ? ST77XX_YELLOW : ST77XX_WHITE);
            tft.print(" ");
            tft.print(controlItems[i]);
        }
    }
}

void drawPumpControlMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" УПРАВЛЕНИЕ ПОЛИВОМ ");
    
    int startY = 35;
    for(int i = 0; i < 5; i++) {
        tft.setCursor(15, startY + i * 15);
        
        if(i == pumpControlItem) {
            tft.setTextColor(ST77XX_WHITE);
            tft.print(">");
        } else {
            tft.setTextColor(i == 4 ? ST77XX_YELLOW : ST77XX_WHITE);
            tft.print(" ");
        }
        
        if(i < 3) {
            tft.print(pumpItems[i]);
            tft.print(": ");
            tft.print(digitalRead(i == 0 ? RELAY_PIN_1 : (i == 1 ? RELAY_PIN_2 : RELAY_PIN_3)) ? "Вкл" : "Выкл");
        } else if(i == 3) {
            tft.print(pumpItems[i]);
            tft.print(": ");
            tft.print(manualWateringControl ? "Ручной" : "Авто");
        } else {
            tft.print(pumpItems[i]);
        }
    }
}

void drawLightControlMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" УПРАВЛЕНИЕ СВЕТОМ ");
    
    tft.setCursor(15, 35);
    tft.setTextColor(lightControlItem == 0 ? ST77XX_WHITE : ST77XX_WHITE);
    tft.print(lightControlItem == 0 ? ">" : " ");
    tft.print("Состояние: ");
    tft.print(lightState ? "ВКЛ" : "ВЫКЛ");
    
    tft.setCursor(15, 50);
    tft.setTextColor(lightControlItem == 1 ? ST77XX_WHITE : ST77XX_WHITE);
    tft.print(lightControlItem == 1 ? ">" : " ");
    tft.print("Режим: ");
    tft.print(manualLightControl ? "Ручной" : "Авто");
    
    tft.setCursor(15, 65);
    tft.print("Датчик: ");
    tft.print(lightDigitalValue ? "Темно" : "Светло");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(lightControlItem == 2 ? ST77XX_WHITE : ST77XX_YELLOW);
    tft.print(lightControlItem == 2 ? ">" : " ");
    tft.print("Назад");
}

void drawWindowControlMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" УПРАВЛЕНИЕ ФОРТОЧКОЙ ");
    
    tft.setCursor(15, 35);
    tft.setTextColor(windowControlItem == 0 ? ST77XX_WHITE : ST77XX_WHITE);
    tft.print(windowControlItem == 0 ? ">" : " ");
    tft.print("Состояние: ");
    tft.print(windowOpenState ? "ОТКРЫТА" : "ЗАКРЫТА");
    
    tft.setCursor(15, 50);
    tft.setTextColor(windowControlItem == 1 ? ST77XX_WHITE : ST77XX_WHITE);
    tft.print(windowControlItem == 1 ? ">" : " ");
    tft.print("Режим: ");
    tft.print(manualWindowControl ? "Ручной" : "Авто");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(windowControlItem == 2 ? ST77XX_WHITE : ST77XX_YELLOW);
    tft.print(windowControlItem == 2 ? ">" : " ");
    tft.print("Назад");
}

void drawSoilMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ВЛАЖНОСТЬ ПОЧВЫ ");
    
    tft.setCursor(15, 35);
    tft.print("Сенсор 1: ");
    tft.print(soilMoisture1);
    tft.print(" (");
    tft.print(soilMoisture1 > settings.soilThreshold ? "Сухо" : "Влажно");
    tft.print(")");
    
    tft.setCursor(15, 50);
    tft.print("Сенсор 2: ");
    tft.print(soilMoisture2);
    tft.print(" (");
    tft.print(soilMoisture2 > settings.soilThreshold ? "Сухо" : "Влажно");
    tft.print(")");
    
    tft.setCursor(15, 65);
    tft.print("Сенсор 3: ");
    tft.print(soilMoisture3);
    tft.print(" (");
    tft.print(soilMoisture3 > settings.soilThreshold ? "Сухо" : "Влажно");
    tft.print(")");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawTempMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ТЕМПЕРАТУРА ");
    
    tft.setCursor(15, 35);
    tft.print("Текущая: ");
    tft.print(currentTemperature, 1);
    tft.print(" C");
    
    tft.setCursor(15, 50);
    tft.print("Порог: ");
    tft.print(settings.temperatureThreshold, 1);
    tft.print(" C");
    
    tft.setCursor(15, 65);
    tft.print("Состояние: ");
    tft.print(currentTemperature > settings.temperatureThreshold ? "Выше нормы" : "Норма");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawHumidityMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ВЛАЖНОСТЬ ВОЗДУХА ");
    
    tft.setCursor(15, 35);
    tft.print("Текущая: ");
    tft.print(currentHumidity, 1);
    tft.print(" %");
    
    tft.setCursor(15, 50);
    tft.print("Порог: ");
    tft.print(settings.humidityThreshold, 1);
    tft.print(" %");
    
    tft.setCursor(15, 65);
    tft.print("Состояние: ");
    tft.print(currentHumidity > settings.humidityThreshold ? "Выше нормы" : "Норма");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void drawLightMenu() {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 15);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" ОСВЕЩЕННОСТЬ ");
    
    tft.setCursor(15, 35);
    tft.print("Состояние: ");
    tft.print(lightDigitalValue ? "Темно" : "Светло");
    
    tft.setCursor(15, 50);
    tft.print("Режим: ");
    tft.print(settings.lightThreshold ? "Ночной" : "Дневной");
    
    tft.setCursor(15, 65);
    tft.print("Лампа: ");
    tft.print(lightState ? "Вкл" : "Выкл");
    
    tft.setCursor(15, tft.height() - 20);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("> Назад");
}

void updateScreen() {
    if(!getScreenUpdateFlag()) return;
    
    switch(currentScreen) {
        case MAIN_MENU: drawMainMenu(); break;
        case SETTINGS_MENU: drawSettingsMenu(); break;
        case CONTROL_MENU: drawControlMenu(); break;
        case PUMP_CONTROL: drawPumpControlMenu(); break;
        case LIGHT_CONTROL: drawLightControlMenu(); break;
        case WINDOW_CONTROL: drawWindowControlMenu(); break;
        case SOIL_MENU: drawSoilMenu(); break;
        case TEMP_MENU: drawTempMenu(); break;
        case HUMIDITY_MENU: drawHumidityMenu(); break;
        case LIGHT_MENU: drawLightMenu(); break;
    }
    
    setScreenUpdateFlag(false);
}

bool getScreenUpdateFlag() {
    return screenNeedsUpdate;
}

void setScreenUpdateFlag(bool state) {
    screenNeedsUpdate = state;
}