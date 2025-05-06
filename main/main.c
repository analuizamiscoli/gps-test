#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "esp_log.h"

#define UART_NUM UART_NUM_1
#define BUF_SIZE (1024)

static const char *TAG = "GPS";

void parse_gpgga(const char *nmea_sentence) {
    // Exemplo: $GPGGA,123519,4807.038,N,01131.000,E,1,...
    char *token;
    char buffer[128];
    strncpy(buffer, nmea_sentence, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';

    token = strtok(buffer, ",");

    int field = 0;
    char time[16] = "";
    char lat[16] = "", lat_dir = 'N';
    char lon[16] = "", lon_dir = 'E';
    int fix = 0;

    while (token != NULL) {
        switch (field) {
            case 1: strncpy(time, token, sizeof(time)); break;
            case 2: strncpy(lat, token, sizeof(lat)); break;
            case 3: lat_dir = token[0]; break;
            case 4: strncpy(lon, token, sizeof(lon)); break;
            case 5: lon_dir = token[0]; break;
            case 6: fix = atoi(token); break;
        }
        token = strtok(NULL, ",");
        field++;
    }

    if (fix == 0) {
        ESP_LOGW(TAG, "Aguardando fix de satélite...");
    } else {
        double lat_deg = atof(lat) / 100.0;
        int lat_int = (int)lat_deg;
        double lat_min = (atof(lat) - lat_int * 100) / 60.0;
        double latitude = lat_int + lat_min;
        if (lat_dir == 'S') latitude *= -1;

        double lon_deg = atof(lon) / 100.0;
        int lon_int = (int)lon_deg;
        double lon_min = (atof(lon) - lon_int * 100) / 60.0;
        double longitude = lon_int + lon_min;
        if (lon_dir == 'W') longitude *= -1;

        ESP_LOGI(TAG, "Latitude: %.6f, Longitude: %.6f", latitude, longitude);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Inicializando GPS...");

    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, 21,14, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uint8_t data[BUF_SIZE];

    while (1) {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(1000));
        if (len > 0) {
            data[len] = '\0';
            char *line = strtok((char *)data, "\r\n");
            while (line != NULL) {
                if (strstr(line, "$GPGGA")) {
                    parse_gpgga(line);
                } else if (strstr(line, "$GPTXT")) {
                    ESP_LOGI(TAG, "[INFO] %s", line);
                }
                line = strtok(NULL, "\r\n");
            }
        }
    }
}