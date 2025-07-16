#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <GyverEncoder.h>
#include "main.h"

// Пины дисплея
#define TFT_CS   15
#define TFT_RST  2
#define TFT_DC   12
#define TFT_SCLK 14
#define TFT_MOSI 13

// Пины энкодера
#define ENCODER_PIN_A 26
#define ENCODER_PIN_B 25
#define ENCODER_BUTTON 27
#define ENCODER_DEBOUNCE_TIME 50

// Константы автопрокрутки
#define AUTOSCROLL_DELAY 500
#define AUTOSCROLL_INTERVAL 200

extern Adafruit_ST7735 tft;
extern Encoder encoder;

enum ScreenState {
    MAIN_MENU,
    SOIL_MENU,
    TEMP_MENU,
    HUMIDITY_MENU,
    LIGHT_MENU,
    CONTROL_MENU,
    PUMP_CONTROL,
    LIGHT_CONTROL,
    WINDOW_CONTROL,
    SETTINGS_MENU
};

enum SettingsSubState {
    SETTINGS_MAIN,
    SETTINGS_TEMP,
    SETTINGS_HUM,
    SETTINGS_SOIL,
    SETTINGS_LIGHT
};

void setupScreen();
void handleEncoder();
void updateScreen();
void selectMenuItem(int item);
bool getScreenUpdateFlag();
void setScreenUpdateFlag(bool state);

void drawMainMenu();
void drawSoilMenu();
void drawTempMenu();
void drawHumidityMenu();
void drawLightMenu();
void drawControlMenu();
void drawSettingsMenu();
void drawPumpControlMenu();
void drawLightControlMenu();
void drawWindowControlMenu();
void drawSettingsTempMenu();
void drawSettingsHumMenu();
void drawSettingsSoilMenu();
void drawSettingsLightMenu();

void handleLightControl();
void toggleLightMode();
void toggleLightState();

void navigateMenu(bool direction);
void togglePump(int pumpNum);
void toggleLight();
void toggleLightMode();
void toggleWindow();
void toggleWindowMode();
void adjustSetting(int direction);

#endif