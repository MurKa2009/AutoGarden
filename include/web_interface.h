#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include "main.h"

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Умная теплица</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.10.0/font/bootstrap-icons.css">
    <style>
        .card { margin-bottom: 20px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
        .card-header { font-weight: 600; }
        .sensor-value { font-size: 1.4rem; font-weight: 700; }
        .progress { height: 10px; }
        .control-btn { width: 100%; }
        .tab-content { padding-top: 15px; }
        .status-badge { font-size: 0.9rem; }
        .soil-container { margin-bottom: 15px; }
        .sensor-card { height: 100%; }
        .sensor-icon { font-size: 2rem; margin-bottom: 10px; }
        .auto-mode-badge { font-size: 0.8rem; }
    </style>
</head>
<body>
    <div class="container py-4">
        <div class="d-flex justify-content-between align-items-center mb-4">
            <h1><i class="bi bi-tree"></i> Умная теплица</h1>
            <div class="text-end">
                <small class="text-muted">IP: %IP%</small><br>
                <span class="badge bg-%WIFI_STATUS%">WiFi: %WIFI_STRENGTH%</span>
            </div>
        </div>

        <ul class="nav nav-tabs" id="myTab" role="tablist">
            <li class="nav-item">
                <button class="nav-link active" id="status-tab" data-bs-toggle="tab" data-bs-target="#status">Состояние</button>
            </li>
            <li class="nav-item">
                <button class="nav-link" id="control-tab" data-bs-toggle="tab" data-bs-target="#control">Управление</button>
            </li>
            <li class="nav-item">
                <button class="nav-link" id="settings-tab" data-bs-toggle="tab" data-bs-target="#settings">Настройки</button>
            </li>
        </ul>

        <div class="tab-content">
            <div class="tab-pane fade show active" id="status">
                <div class="row mt-3">
                    <div class="col-md-4">
                        <div class="card sensor-card">
                            <div class="card-body text-center">
                                <div class="sensor-icon text-primary">
                                    <i class="bi bi-thermometer-half"></i>
                                </div>
                                <h5 class="card-title">Температура</h5>
                                <div class="sensor-value" id="temperature">%TEMPERATURE% °C</div>
                                <div class="progress mt-2">
                                    <div id="temp-progress" class="progress-bar bg-primary" style="width: %TEMP_PROGRESS%%"></div>
                                </div>
                                <span class="badge bg-%TEMP_AUTO_MODE% auto-mode-badge mt-2">
                                    %TEMP_AUTO_TEXT%
                                </span>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-4">
                        <div class="card sensor-card">
                            <div class="card-body text-center">
                                <div class="sensor-icon text-info">
                                    <i class="bi bi-droplet-half"></i>
                                </div>
                                <h5 class="card-title">Влажность</h5>
                                <div class="sensor-value" id="humidity">%HUMIDITY% %</div>
                                <div class="progress mt-2">
                                    <div id="hum-progress" class="progress-bar bg-info" style="width: %HUM_PROGRESS%%"></div>
                                </div>
                                <span class="badge bg-%HUM_AUTO_MODE% auto-mode-badge mt-2">
                                    %HUM_AUTO_TEXT%
                                </span>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-4">
                        <div class="card sensor-card">
                            <div class="card-body text-center">
                                <div class="sensor-icon text-warning">
                                    <i class="bi bi-brightness-high"></i>
                                </div>
                                <h5 class="card-title">Освещение</h5>
                                <div>
                                    <span id="light-value" class="badge bg-%LIGHT_VALUE%">%LIGHT_VALUE_TEXT%</span>
                                </div>
                                <div class="mt-2">
                                    Режим: <span id="light-mode" class="badge bg-%LIGHT_MODE%">%LIGHT_MODE_TEXT%</span>
                                </div>
                                <span class="badge bg-%LIGHT_AUTO_MODE% auto-mode-badge mt-2">
                                    %LIGHT_AUTO_TEXT%
                                </span>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="row mt-3">
                    <div class="col-md-4">
                        <div class="card">
                            <div class="card-header">
                                <i class="bi bi-moisture"></i> Влажность почвы 1
                            </div>
                            <div class="card-body">
                                <div class="soil-container">
                                    <div class="d-flex justify-content-between">
                                        <span>Значение:</span>
                                        <span id="soil1-value" class="badge bg-%SOIL1_COLOR%">%SOIL1_VALUE%</span>
                                    </div>
                                    <div class="progress mt-2">
                                        <div id="soil1-progress" class="progress-bar bg-success" style="width: %SOIL1_PROGRESS%%"></div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-4">
                        <div class="card">
                            <div class="card-header">
                                <i class="bi bi-moisture"></i> Влажность почвы 2
                            </div>
                            <div class="card-body">
                                <div class="soil-container">
                                    <div class="d-flex justify-content-between">
                                        <span>Значение:</span>
                                        <span id="soil2-value" class="badge bg-%SOIL2_COLOR%">%SOIL2_VALUE%</span>
                                    </div>
                                    <div class="progress mt-2">
                                        <div id="soil2-progress" class="progress-bar bg-success" style="width: %SOIL2_PROGRESS%%"></div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-4">
                        <div class="card">
                            <div class="card-header">
                                <i class="bi bi-moisture"></i> Влажность почвы 3
                            </div>
                            <div class="card-body">
                                <div class="soil-container">
                                    <div class="d-flex justify-content-between">
                                        <span>Значение:</span>
                                        <span id="soil3-value" class="badge bg-%SOIL3_COLOR%">%SOIL3_VALUE%</span>
                                    </div>
                                    <div class="progress mt-2">
                                        <div id="soil3-progress" class="progress-bar bg-success" style="width: %SOIL3_PROGRESS%%"></div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <div class="tab-pane fade" id="control">
                <div class="row">
                    <div class="col-md-6">
                        <div class="card">
                            <div class="card-header">
                                <i class="bi bi-droplet"></i> Управление поливом
                            </div>
                            <div class="card-body">
                                <div class="mb-3">
                                    <label class="form-label">Режим работы:</label>
                                    <div class="btn-group w-100">
                                        <button onclick="setMode('water', true)" class="btn btn-%WATER_AUTO%">Авто</button>
                                        <button onclick="setMode('water', false)" class="btn btn-%WATER_MANUAL%">Ручной</button>
                                    </div>
                                </div>
                                <div class="mb-3" id="water-controls" style="display: %WATER_CONTROLS_DISPLAY%">
                                    <label class="form-label">Ручное управление:</label>
                                    <div class="row">
                                        <div class="col-md-4 mb-2">
                                            <button onclick="controlPump(1, 'on')" class="btn btn-%PUMP1_ON% w-100">Помпа 1 Вкл</button>
                                            <button onclick="controlPump(1, 'off')" class="btn btn-%PUMP1_OFF% w-100 mt-1">Помпа 1 Выкл</button>
                                        </div>
                                        <div class="col-md-4 mb-2">
                                            <button onclick="controlPump(2, 'on')" class="btn btn-%PUMP2_ON% w-100">Помпа 2 Вкл</button>
                                            <button onclick="controlPump(2, 'off')" class="btn btn-%PUMP2_OFF% w-100 mt-1">Помпа 2 Выкл</button>
                                        </div>
                                        <div class="col-md-4 mb-2">
                                            <button onclick="controlPump(3, 'on')" class="btn btn-%PUMP3_ON% w-100">Помпа 3 Вкл</button>
                                            <button onclick="controlPump(3, 'off')" class="btn btn-%PUMP3_OFF% w-100 mt-1">Помпа 3 Выкл</button>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="card">
                            <div class="card-header">
                                <i class="bi bi-sun"></i> Управление освещением
                            </div>
                            <div class="card-body">
                                <div class="mb-3">
                                    <label class="form-label">Режим работы:</label>
                                    <div class="btn-group w-100">
                                        <button onclick="setMode('light', true)" class="btn btn-%LIGHT_AUTO%">Авто</button>
                                        <button onclick="setMode('light', false)" class="btn btn-%LIGHT_MANUAL%">Ручной</button>
                                    </div>
                                </div>
                                <div class="mb-3" id="light-controls" style="display: %LIGHT_CONTROLS_DISPLAY%">
                                    <label class="form-label">Ручное управление:</label>
                                    <div class="btn-group w-100">
                                        <button onclick="control('light', 'on')" class="btn btn-%LIGHT_ON%">Включить</button>
                                        <button onclick="control('light', 'off')" class="btn btn-%LIGHT_OFF%">Выключить</button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="row mt-3">
                    <div class="col-md-6">
                        <div class="card">
                            <div class="card-header">
                                <i class="bi bi-wind"></i> Управление форточкой
                            </div>
                            <div class="card-body">
                                <div class="mb-3">
                                    <label class="form-label">Режим работы:</label>
                                    <div class="btn-group w-100">
                                        <button onclick="setMode('window', true)" class="btn btn-%WINDOW_AUTO%">Авто</button>
                                        <button onclick="setMode('window', false)" class="btn btn-%WINDOW_MANUAL%">Ручной</button>
                                    </div>
                                </div>
                                <div class="mb-3" id="window-controls" style="display: %WINDOW_CONTROLS_DISPLAY%">
                                    <label class="form-label">Ручное управление:</label>
                                    <div class="btn-group w-100">
                                        <button onclick="control('window', 'open')" class="btn btn-%WINDOW_OPEN%">Открыть</button>
                                        <button onclick="control('window', 'close')" class="btn btn-%WINDOW_CLOSE%">Закрыть</button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <div class="tab-pane fade" id="settings">
                <div class="card">
                    <div class="card-header">
                        <i class="bi bi-sliders"></i> Настройки параметров
                    </div>
                    <div class="card-body">
                        <form onsubmit="saveSettings(this);return false">
                            <div class="mb-3">
                                <label class="form-label">Порог температуры (°C)</label>
                                <input type="number" step="0.1" class="form-control" id="tempThreshold" value="%TEMP_THRESHOLD%">
                            </div>
                            <div class="mb-3">
                                <label class="form-label">Порог влажности (%)</label>
                                <input type="number" step="0.1" class="form-control" id="humThreshold" value="%HUM_THRESHOLD%">
                            </div>
                            <div class="mb-3">
                                <label class="form-label">Порог влажности почвы</label>
                                <input type="number" class="form-control" id="soilThreshold" value="%SOIL_THRESHOLD%">
                                <small class="text-muted">0-4095 (меньше - влажнее)</small>
                            </div>
                            <div class="mb-3">
                                <label class="form-label">Режим освещения</label>
                                <select class="form-select" id="lightMode">
                                    <option value="0" %LIGHT_MODE_0%>Вкл. при свете</option>
                                    <option value="1" %LIGHT_MODE_1%>Вкл. при темноте</option>
                                </select>
                            </div>
                            <button type="submit" class="btn btn-primary w-100">
                                <i class="bi bi-save"></i> Сохранить
                            </button>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js"></script>
    <script>
        function updateSensorData(data) {
            // Обновление данных датчиков
            document.getElementById('temperature').textContent = data.temperature.toFixed(1) + ' °C';
            document.getElementById('humidity').textContent = data.humidity.toFixed(1) + ' %';
            
            // Прогресс-бары
            document.getElementById('temp-progress').style.width = Math.min(100, data.temperature/50*100) + '%';
            document.getElementById('hum-progress').style.width = data.humidity + '%';
            
            // Влажность почвы
            updateSoilMoisture('soil1', data.soil1, data.soilThreshold, data.pump1State);
            updateSoilMoisture('soil2', data.soil2, data.soilThreshold, data.pump2State);
            updateSoilMoisture('soil3', data.soil3, data.soilThreshold, data.pump3State);
            
            // Освещение
            const lightValue = data.lightValue ? 'DARK' : 'LIGHT';
            const lightMode = data.lightMode ? 'Ночной' : 'Дневной';
            document.getElementById('light-value').textContent = lightValue;
            document.getElementById('light-value').className = 'badge ' + (data.lightValue ? 'bg-dark' : 'bg-warning');
            document.getElementById('light-mode').textContent = lightMode;
            document.getElementById('light-mode').className = 'badge ' + (data.lightMode ? 'bg-dark' : 'bg-light text-dark');
            
            // Режимы автоматического управления
            updateAutoModeBadge('temp', data.autoTemp, 'Температура');
            updateAutoModeBadge('hum', data.autoHum, 'Влажность');
            updateAutoModeBadge('light', !data.manualLight, 'Освещение');
            
            // Управление поливом
            document.getElementById('water-controls').style.display = data.manualWater ? 'block' : 'none';
            updateButtonGroupState('water', data.manualWater);
            
            // Управление светом
            document.getElementById('light-controls').style.display = data.manualLight ? 'block' : 'none';
            updateButtonGroupState('light', data.manualLight);
            
            // Управление форточкой
            document.getElementById('window-controls').style.display = data.manualWindow ? 'block' : 'none';
            updateButtonGroupState('window', data.manualWindow);
            
            // Состояние кнопок помп
            updatePumpButton(1, data.pump1State);
            updatePumpButton(2, data.pump2State);
            updatePumpButton(3, data.pump3State);
            
            // Состояние кнопок света
            updateControlButton('light', data.lightState);
            
            // Состояние кнопок форточки
            updateControlButton('window', data.windowState);
        }

        function updateSoilMoisture(prefix, value, threshold, pumpState) {
            const element = document.getElementById(prefix+'-value');
            element.textContent = value;
            
            // Определяем цвет в зависимости от состояния помпы и влажности
            let colorClass;
            if (pumpState) {
                colorClass = 'bg-primary'; // Синий - помпа включена
            } else if (value > threshold) {
                colorClass = 'bg-danger'; // Красный - нужно поливать
            } else {
                colorClass = 'bg-success'; // Зеленый - норма
            }
            
            element.className = 'badge ' + colorClass;
            
            // Прогресс-бар (инвертированный, так как меньше значение - больше влажность)
            const percent = Math.min(100, (4095 - value) / (4095 - threshold) * 100);
            document.getElementById(prefix+'-progress').style.width = percent + '%';
        }

        function updateAutoModeBadge(type, isAuto, text) {
            const element = document.querySelector(`.badge.bg-${type}`);
            if (element) {
                element.className = `badge bg-${isAuto ? 'success' : 'warning'} auto-mode-badge mt-2`;
                element.textContent = isAuto ? `${text}: Авто` : `${text}: Ручной`;
            }
        }

        function updateButtonGroupState(type, isManual) {
            const group = document.querySelector(`#control [data-type="${type}"]`);
            if (group) {
                const autoBtn = group.querySelector('.btn:first-child');
                const manualBtn = group.querySelector('.btn:last-child');
                
                autoBtn.className = isManual ? 'btn btn-outline-primary' : 'btn btn-primary';
                manualBtn.className = isManual ? 'btn btn-primary' : 'btn btn-outline-primary';
            }
        }

        function updatePumpButton(pumpNum, isOn) {
            const onBtn = document.querySelector(`.btn-pump${pumpNum}-on`);
            const offBtn = document.querySelector(`.btn-pump${pumpNum}-off`);
            
            if (onBtn) onBtn.className = `btn btn-${isOn ? 'primary' : 'outline-primary'} w-100 btn-pump${pumpNum}-on`;
            if (offBtn) offBtn.className = `btn btn-${!isOn ? 'danger' : 'outline-danger'} w-100 mt-1 btn-pump${pumpNum}-off`;
        }

        function updateControlButton(type, isOn) {
            const onBtn = document.querySelector(`.btn-${type}-on`);
            const offBtn = document.querySelector(`.btn-${type}-off`);
            
            if (onBtn) onBtn.className = `btn btn-${isOn ? 'success' : 'outline-success'} btn-${type}-on`;
            if (offBtn) offBtn.className = `btn btn-${!isOn ? 'danger' : 'outline-danger'} btn-${type}-off`;
        }

        function controlPump(pumpNum, action) {
            fetch(`/control?type=pump${pumpNum}&action=${action}`)
                .then(response => {
                    if(response.ok) updateData();
                });
        }

        function control(type, action) {
            fetch(`/control?type=${type}&action=${action}`)
                .then(response => {
                    if(response.ok) updateData();
                });
        }

        function setMode(type, auto) {
            fetch(`/control?type=${type}&action=${auto ? 'auto' : 'manual'}`)
                .then(response => {
                    if(response.ok) updateData();
                });
        }

        function saveSettings(form) {
            const data = {
                temp: parseFloat(form.tempThreshold.value),
                hum: parseFloat(form.humThreshold.value),
                soil: parseInt(form.soilThreshold.value),
                light: parseInt(form.lightMode.value)
            };
            
            fetch('/settings', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            }).then(response => {
                if(response.ok) {
                    alert('Настройки сохранены!');
                    updateData();
                } else {
                    alert('Ошибка сохранения!');
                }
            });
        }

        function updateData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => updateSensorData(data));
        }

        // Обновляем данные каждые 3 секунды
        setInterval(updateData, 3000);
        
        // Инициализация при загрузке страницы
        document.addEventListener('DOMContentLoaded', function() {
            updateData();
            
            // Инициализация вкладок Bootstrap
            const tabElms = document.querySelectorAll('button[data-bs-toggle="tab"]');
            tabElms.forEach(tabElm => {
                tabElm.addEventListener('click', function(event) {
                    const tabTrigger = new bootstrap.Tab(this);
                    tabTrigger.show();
                });
            });
        });
    </script>
</body>
</html>
)=====";

void setupWebServer();
void updateSoilMoisturePlaceholders(String& html, const String& num, int value);

#endif