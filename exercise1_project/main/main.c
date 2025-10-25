#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define constants for the 3 LEDs
#define LED1_GPIO   GPIO_NUM_2
#define LED2_GPIO   GPIO_NUM_4
#define LED3_GPIO   GPIO_NUM_5
#define BLINK_PERIOD 1000 // 1 second (1000ms)

static const char *TAG = "LED_CONTROL";

/**
 * @brief Initialize GPIOs for the LEDs
 */
void led_init(void) {
    ESP_LOGI(TAG, "Initializing LEDs on GPIO %d, %d, %d", LED1_GPIO, LED2_GPIO, LED3_GPIO);
    
    // Create GPIO configuration structure
    gpio_config_t io_conf = {0};
    
    // Define the pins to be used (bit mask)
    io_conf.pin_bit_mask = (1ULL << LED1_GPIO) | (1ULL << LED2_GPIO) | (1ULL << LED3_GPIO);
    
    // Set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    
    // Disable pull-up/pull-down
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    
    // Disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    // Apply configuration
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    
    // Set initial state (all LEDs OFF)
    gpio_set_level(LED1_GPIO, 0);
    gpio_set_level(LED2_GPIO, 0);
    gpio_set_level(LED3_GPIO, 0);
    
    ESP_LOGI(TAG, "LED initialization completed");
}

/**
 * @brief Control all LEDs (ON/OFF)
 * @param state: 1 = LEDs ON, 0 = LEDs OFF
 */
void leds_control(int state) {
    gpio_set_level(LED1_GPIO, state);
    gpio_set_level(LED2_GPIO, state);
    gpio_set_level(LED3_GPIO, state);
    ESP_LOGI(TAG, "LEDs %s", state ? "ON" : "OFF");
}

/**
 * @brief Task for blinking all LEDs together
 */
void leds_blink_task(void *pvParameters) {
    int led_state = 0;
    
    while (1) {
        // Toggle LED state
        led_state = !led_state;
        leds_control(led_state);
        
        // Delay
        vTaskDelay(pdMS_TO_TICKS(BLINK_PERIOD));
    }
}

/**
 * @brief Main application entry point
 */
void app_main(void) {
    ESP_LOGI(TAG, "ESP32 Multi-LED Control Demo Started");
    
    // Initialize LEDs
    led_init();
    
    // Create LED blink task
    xTaskCreate(
        leds_blink_task,         // Function pointer
        "leds_blink_task",       // Task name
        2048,                    // Stack size (bytes)
        NULL,                    // Parameters
        5,                       // Priority
        NULL                     // Task handle
    );
    
    ESP_LOGI(TAG, "LEDs blink task created");
}