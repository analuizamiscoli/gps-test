# ESP32-S3 - GPS Test

Este projeto é um teste simples de leitura de dados do módulo GPS NEO-6M via UART usando ESP32-S3 com ESP-IDF.

## Como usar

1. Clone este repositório
2. Compile e grave com idf.py build flash monitor
3. Verifique a saída serial para latitude e longitude

## Protocolo usado

- Sentença NMEA: $GPGGA

## Componentes

- ESP32-S3
- Módulo GPS (ex: NEO-6M)
- Comunicação UART (TX do GPS no GPIO 21 do ESP32-S3)