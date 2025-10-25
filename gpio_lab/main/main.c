#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// กำหนดขา GPIO สำหรับ LED ทั้ง 3 ดวง
#define LED1_GPIO GPIO_NUM_2
#define LED2_GPIO GPIO_NUM_4
#define LED3_GPIO GPIO_NUM_5
#define BLINK_PERIOD 500 // หน่วงเวลา 0.5 วินาที

static const char *TAG = "MULTIPLE_LEDS";

/**
 * @brief กำหนดค่าเริ่มต้นสำหรับ GPIO ของ LED ทั้ง 3 ดวง
 */
void leds_init(void) {
    ESP_LOGI(TAG, "Initializing LEDs on GPIO %d, %d, %d", LED1_GPIO, LED2_GPIO, LED3_GPIO);
    
    gpio_config_t io_conf = {
        // ใช้ bit mask เพื่อกำหนดขา GPIO ทั้ง 3 ขาพร้อมกัน
        .pin_bit_mask = (1ULL << LED1_GPIO) | (1ULL << LED2_GPIO) | (1ULL << LED3_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // ตั้งค่าเริ่มต้นให้ LED ทุกดวงดับ
    gpio_set_level(LED1_GPIO, 0);
    gpio_set_level(LED2_GPIO, 0);
    gpio_set_level(LED3_GPIO, 0);

    ESP_LOGI(TAG, "LEDs initialization completed");
}

/**
 * @brief Task สำหรับทำให้ LED ติดทีละดวงเรียงกันไป
 */
void led_pattern_task(void *pvParameters) {
    while (1) {
        ESP_LOGI(TAG, "LED 1 ON");
        gpio_set_level(LED1_GPIO, 1); // เปิด LED1
        gpio_set_level(LED2_GPIO, 0);
        gpio_set_level(LED3_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD));

        ESP_LOGI(TAG, "LED 2 ON");
        gpio_set_level(LED1_GPIO, 0);
        gpio_set_level(LED2_GPIO, 1); // เปิด LED2
        gpio_set_level(LED3_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD));

        ESP_LOGI(TAG, "LED 3 ON");
        gpio_set_level(LED1_GPIO, 0);
        gpio_set_level(LED2_GPIO, 0);
        gpio_set_level(LED3_GPIO, 1); // เปิด LED3
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD));
    }
}

void app_main(void) {
    leds_init();
    xTaskCreate(led_pattern_task, "led_pattern_task", 2048, NULL, 5, NULL);
}