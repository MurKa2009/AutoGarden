#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <Preferences.h>

struct Settings {
    float temperatureThreshold;
    float humidityThreshold;
    int soilThreshold;
    bool lightThreshold;
    bool manualLightControl;
};

extern Settings settings;

void loadSettings();
void saveSettings();

#endif