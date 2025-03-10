# Inclusivity Interactive Maps

## Запуск
Поддерживаются: Ubuntu 20.04, 22.04, 24.04

1. Установить зависимости
```bash
chmod +x build.sh
./build.sh
```
2. Запустить бд и указать в *config-vars.yaml* параметры для ее подключения
3. Запустить
```bash
./build/IIM --config <path/to/the/static_config.yaml> --config_vars <path/to/the/config_vars.yaml>
```