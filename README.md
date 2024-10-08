# Monitoring Activity

Monitoring Activity — это приложение, состоящее из серверной и клиентской частей, предназначенное для мониторинга активности удалённых компьютеров. Клиентская часть автоматически передаёт информацию о компьютере и пользователе на сервер, а также может отправлять скриншоты по запросу.

## Возможности
- Получение информации о подключённых клиентах, таких как имя пользователя, имя компьютера и IP-адрес.
- Запрос скриншотов с удалённых компьютеров.
- Добавление клиентского приложения в автозагрузку при старте системы.

## Установка

### Способ 1: Сборка из исходников
1. Скачайте исходные файлы с [GitHub репозитория](https://github.com/lTETHERl/Monitoring-Activity).
2. Откройте файл проекта `MonitoringActivity.sln` в Visual Studio.
3. Соберите решение, выбрав конфигурацию `Release` или `Debug`.
4. Запустите исполняемый файл **Client.exe** для клиентской части и **Server.exe** для серверной.

### Способ 2: Использование готовых бинарных файлов
1. Перейдите на [страницу релизов](https://github.com/lTETHERl/Monitoring-Activity/releases).
2. Скачайте архив с последней версией.
3. Разархивируйте файлы.
4. Запустите **Client.exe** и **Server.exe** в любом порядке.

## Использование
1. Запустите **Client.exe** на удалённом компьютере. По умолчанию клиентское приложение подключается к серверу с IP-адресом `127.0.0.1` и портом `8080`. Для изменения этих параметров необходимо изменить соответствующие значения в исходном коде или использовать параметры командной строки (если реализовано).
2. Запустите **Server.exe**. Сервер начинает прослушивать подключения на порту `8080` (по умолчанию).
3. В главном окне серверного приложения вы увидите список всех подключённых клиентов с возможностью запроса скриншотов.

> **Примечание:** Сервер и клиент могут быть запущены в любом порядке. Клиентская часть при запуске будет пытаться подключиться к серверу автоматически.

## Дополнительная информация
- Клиентская часть приложения добавляется в автозагрузку при первом запуске и будет автоматически запускаться при старте операционной системы.
- Путь к клиентскому приложению в автозагрузке сохраняется в реестре по следующему пути:  
  `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run`

Для дополнительной информации и изменений в конфигурации смотрите исходный код и комментарии в файлах `client.cpp` и `server.cpp`.
