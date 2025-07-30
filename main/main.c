
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void app_main(void) {
    ESP_LOGI("ESP", "Node Booting...");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("ESP", "Running...");
    }
}
