#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_timer.h"

#define LED1_GPIO   GPIO_NUM_2
#define LED2_GPIO   GPIO_NUM_4
#define LED3_GPIO   GPIO_NUM_5

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_1          LEDC_CHANNEL_0
#define LEDC_CHANNEL_2          LEDC_CHANNEL_1
#define LEDC_CHANNEL_3          LEDC_CHANNEL_2
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // 8192 levels
#define LEDC_FREQUENCY          (5000) // 5 kHz
#define MAX_DUTY                (8191)

#define FADE_TIME_MS            1000 // Fade time in milliseconds
#define PATTERN_DELAY_MS        1000 // Delay between patterns

// Repetitions for each pattern
#define KNIGHT_RIDER_REPS 5
#define BINARY_COUNTER_REPS 3
#define RANDOM_BLINK_REPS 15

static const char *TAG = "BREATHING_PATTERNS";

// Array of channels for convenience
static const ledc_channel_t led_channels[] = {LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3};

/**
 * @brief Initialize 3 LEDC channels for the 3 LEDs
 */
void leds_pwm_init(void) {
    // Configure the timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configure channel 1
    ledc_channel_config_t ledc_channel_1 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED1_GPIO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_1));

    // Configure channel 2
    ledc_channel_config_t ledc_channel_2 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED2_GPIO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_2));

    // Configure channel 3
    ledc_channel_config_t ledc_channel_3 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_3,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED3_GPIO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_3));

    // Initialize fade service
    ESP_ERROR_CHECK(ledc_fade_func_install(0));
    ESP_LOGI(TAG, "LEDs PWM initialized");
}

/**
 * @brief Helper to fade one LED and wait for it to finish
 */
void fade_led(ledc_channel_t channel, uint32_t target_duty, int fade_time) {
    ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_MODE, channel, target_duty, fade_time));
    ESP_ERROR_CHECK(ledc_fade_start(LEDC_MODE, channel, LEDC_FADE_WAIT_DONE));
}

/**
 * @brief Helper to start fading multiple LEDs without waiting
 */
void fade_leds_start(uint32_t duty1, uint32_t duty2, uint32_t duty3, int fade_time) {
    ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL_1, duty1, fade_time);
    ledc_fade_start(LEDC_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL_2, duty2, fade_time);
    ledc_fade_start(LEDC_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL_3, duty3, fade_time);
    ledc_fade_start(LEDC_MODE, LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
    
    // Wait for all fades to complete
    vTaskDelay(pdMS_TO_TICKS(fade_time + 50));
}


/**
 * @brief Breathing Knight Rider Pattern
 */
void run_breathing_knight_rider(void) {
    ESP_LOGI(TAG, "Running Breathing Knight Rider...");
    int sequence[] = {0, 1, 2, 1};
    int sequence_len = sizeof(sequence) / sizeof(sequence[0]);
    for (int rep = 0; rep < KNIGHT_RIDER_REPS; rep++) {
        for (int i = 0; i < sequence_len; i++) {
            // Fade up
            fade_led(led_channels[sequence[i]], MAX_DUTY, FADE_TIME_MS / 2);
            // Fade down
            fade_led(led_channels[sequence[i]], 0, FADE_TIME_MS / 2);
        }
    }
}

/**
 * @brief Breathing Binary Counter Pattern
 */
void run_breathing_binary_counter(void) {
    ESP_LOGI(TAG, "Running Breathing Binary Counter...");
    for (int rep = 0; rep < BINARY_COUNTER_REPS; rep++) {
        for (int i = 0; i < 8; i++) {
            uint32_t d1 = (i & 1) ? MAX_DUTY : 0;
            uint32_t d2 = (i & 2) ? MAX_DUTY : 0;
            uint32_t d3 = (i & 4) ? MAX_DUTY : 0;
            fade_leds_start(d1, d2, d3, FADE_TIME_MS);
            vTaskDelay(pdMS_TO_TICKS(500)); // Hold the state
        }
    }
}

/**
 * @brief Breathing Random Blinking Pattern
 */
void run_breathing_random(void) {
    ESP_LOGI(TAG, "Running Breathing Random Blinking...");
    for (int rep = 0; rep < RANDOM_BLINK_REPS; rep++) {
        int random_state = rand() % 8;
        uint32_t d1 = (random_state & 1) ? MAX_DUTY : 0;
        uint32_t d2 = (random_state & 2) ? MAX_DUTY : 0;
        uint32_t d3 = (random_state & 4) ? MAX_DUTY : 0;
        fade_leds_start(d1, d2, d3, FADE_TIME_MS);
    }
}

/**
 * @brief Main task to cycle through patterns
 */
void patterns_task(void *pvParameters) {
    while (1) {
        run_breathing_knight_rider();
        vTaskDelay(pdMS_TO_TICKS(PATTERN_DELAY_MS));

        run_breathing_binary_counter();
        vTaskDelay(pdMS_TO_TICKS(PATTERN_DELAY_MS));

        run_breathing_random();
        vTaskDelay(pdMS_TO_TICKS(PATTERN_DELAY_MS));
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting Exercise 3: Breathing LED Patterns");
    leds_pwm_init();
    srand(esp_timer_get_time());

    xTaskCreate(patterns_task, "patterns_task", 4096, NULL, 5, NULL);
}