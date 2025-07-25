# Auto Garden: Умная теплица

## Описание проекта
Auto Garden — это проект умной теплицы, предназначенный для автоматизации и упрощения процесса выращивания растений. Система позволяет контролировать и управлять ключевыми параметрами теплицы, обеспечивая оптимальные условия для роста растений.

## Возможности

| Возможность | Описание |
|-------------|----------|
| Управление поливом | Возможность раздельного управления поливом для трёх грядок. |
| Автоматическое проветривание | Система автоматически проветривает теплицу для поддержания оптимального микроклимата. |
| Управление освещением | Включение и выключение освещения в зависимости от времени суток или по расписанию. |
| Ручное управление | Возможность вручную включать помпы для полива, лампочки, а также открывать форточку. |

## Интерфейсы управления

| Интерфейс | Описание |
|-----------|----------|
| Веб-интерфейс | Позволяет удалённо управлять системой через интернет. |
| LCD экран | Локальное управление системой непосредственно в теплице. |

## Установка и Настройка
1. Подключите все компоненты системы согласно схеме подключения.
   
2. [**Установите visual strudio**](https://code.visualstudio.com/) **и** [**platform io**](https://platformio.org)

3. **Клонируйте репозиторий**:
    ```bash
    git clone https://github.com/MurKa2009/
    ```

4. **Загрузите прошивку на ESP32**:
    - Подключите ESP32 к вашему компьютеру.
    - Используйте PlatformIO для загрузки прошивки.

5. **Настройка устройства**:
    - Подключитесь к точке доступа с названием `AutoGarden` и паролем `12345678`.
    - Перейдите на IP-адрес `172.8.254.1`.
    - Настройте сеть, к которой будет подключаться система.
    - так же настройку можно провести использую lcd экран и энкодер.

## Использование

Наслаждайтесь автоматизацией!

## Лицензия

Этот проект распространяется под лицензией MIT. Подробности смотрите в файле LICENSE.
