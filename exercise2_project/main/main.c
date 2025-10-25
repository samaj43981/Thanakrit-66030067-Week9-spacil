#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

// Define constants for the 3 LEDs
#define LED1_GPIO   GPIO_NUM_2
#define LED2_GPIO   GPIO_NUM_4
#define LED3_GPIO   GPIO_NUM_5

// Repetitions for each pattern to control duration
#define KNIGHT_RIDER_REPS 10
#define BINARY_COUNTER_REPS 5
#define RANDOM_BLINK_REPS 30

static const char *TAG = "LED_PATTERNS";
static const int leds[] = {LED1_GPIO, LED2_GPIO, LED3_GPIO};
static const int num_leds = sizeof(leds) / sizeof(leds[0]);

/**
 * @brief Initialize GPIOs for the LEDs
 */
void leds_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED1_GPIO) | (1ULL << LED2_GPIO) | (1ULL << LED3_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_LOGI(TAG, "LEDs initialized");
}

/**
 * @brief Turn all LEDs off
 */
void leds_all_off(void) {
    for (int i = 0; i < num_leds; i++) {
        gpio_set_level(leds[i], 0);
    }
}

/**
 * @brief Knight Rider Pattern
 */
void run_knight_rider(void) {
    ESP_LOGI(TAG, "Running Knight Rider pattern...");
    int sequence[] = {0, 1, 2, 1};
    int sequence_len = sizeof(sequence) / sizeof(sequence[0]);
    for(int rep = 0; rep < KNIGHT_RIDER_REPS; rep++) {
        for (int i = 0; i < sequence_len; i++) {
            leds_all_off();
            gpio_set_level(leds[sequence[i]], 1);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

/**
 * @brief Binary Counter Pattern
 */
void run_binary_counter(void) {
    ESP_LOGI(TAG, "Running Binary Counter pattern...");
    for(int rep = 0; rep < BINARY_COUNTER_REPS; rep++) {
        for (int i = 0; i < 8; i++) {
            gpio_set_level(LED1_GPIO, (i & 1) ? 1 : 0); // LSB
            gpio_set_level(LED2_GPIO, (i & 2) ? 1 : 0);
            gpio_set_level(LED3_GPIO, (i & 4) ? 1 : 0); // MSB
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

/**
 * @brief Random Blinking Pattern
 */
void run_random_blinking(void) {
    ESP_LOGI(TAG, "Running Random Blinking pattern...");
    for(int rep = 0; rep < RANDOM_BLINK_REPS; rep++) {
        int random_state = rand() % 8;
        gpio_set_level(LED1_GPIO, (random_state & 1) ? 1 : 0);
        gpio_set_level(LED2_GPIO, (random_state & 2) ? 1 : 0);
        gpio_set_level(LED3_GPIO, (random_state & 4) ? 1 : 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Main task to cycle through patterns
 */
void patterns_task(void *pvParameters) {
    while (1) {
        run_knight_rider();
        leds_all_off();
        vTaskDelay(pdMS_TO_TICKS(1000));

        run_binary_counter();
        leds_all_off();
        vTaskDelay(pdMS_TO_TICKS(1000));

        run_random_blinking();
        leds_all_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting Exercise 2: LED Patterns");
    leds_init();
    // Seed random number generator
    srand(esp_timer_get_time());

    xTaskCreate(patterns_task, "patterns_task", 2048, NULL, 5, NULL);
}