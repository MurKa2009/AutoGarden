#include "settings_manager.h"

Settings settings = {
    .temperatureThreshold = 25.0,
    .humidityThreshold = 70.0,
    .soilThreshold = 2000,
    .lightThreshold = false,
    .manualLightControl = false
};

void loadSettings() {
    Preferences prefs;
    prefs.begin("greenhouse", true);
    settings.temperatureThreshold = prefs.getFloat("temp", 25.0);
    settings.humidityThreshold = prefs.getFloat("hum", 70.0);
    settings.soilThreshold = prefs.getInt("soil", 2000);
    settings.lightThreshold = prefs.getBool("light", false);
    settings.manualLightControl = prefs.getBool("light_manual", false);
    prefs.end();
}

void saveSettings() {
    Preferences prefs;
    prefs.begin("greenhouse", false);
    prefs.putFloat("temp", settings.temperatureThreshold);
    prefs.putFloat("hum", settings.humidityThreshold);
    prefs.putInt("soil", settings.soilThreshold);
    prefs.putBool("light", settings.lightThreshold);
    prefs.putBool("light_manuaal", settings.manualLightControl);
    prefs.end();
}