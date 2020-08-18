/* LEDC (LED Controller) fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of LEDC first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the LEDC channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19/4/5 as LEDC output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    GPIO4/5 are from low speed channel group.
 *
 */
#ifdef CONFIG_IDF_TARGET_ESP32
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (18)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#endif

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH1_GPIO       (4)
#define LEDC_LS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_TEST_CH_NUM_0     (4)

#define LEDC_TEST_DUTY_0       (2500)
#define LEDC_TEST_DUTY_1       (1000)

#define LEDC_TEST_FADE_TIME    (3000) 

void app_main(void)
{
    int ch;

    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_LS_MODE,           // timer mode
        .timer_num = LEDC_LS_TIMER,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);
#ifdef CONFIG_IDF_TARGET_ESP32
    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_HS_MODE;
    ledc_timer.timer_num = LEDC_HS_TIMER;
    ledc_timer_config(&ledc_timer);
#endif
    /*
     * Prepare individual configuration
     * for each channel of LED Controller
     * by selecting:
     * - controller's channel number
     * - output duty cycle, set initially to 0
     * - GPIO number where LED is connected to
     * - speed mode, either high or low
     * - timer servicing selected channel
     *   Note: if different channels use one timer,
     *         then frequency and bit_num of these channels
     *         will be the same
     */
    ledc_channel_config_t ledc_channel_0[LEDC_TEST_CH_NUM_0] = {
   
//#ifdef CONFIG_IDF_TARGET_ESP32
        {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH0_GPIO, //18
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER
        },
    };

    ledc_channel_config_t ledc_channel_1[LEDC_TEST_CH_NUM_1] = {
        {
            .channel    = LEDC_LS_CH1_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_LS_CH1_GPIO, //4
            .speed_mode = LEDC_LS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_LS_TIMER
        },
    };

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_TEST_CH_NUM_0; ch++) {
        ledc_channel_config(&ledc_channel_0[ch]);
    }

    for (ch = 0; ch < LEDC_TEST_CH_NUM_1; ch++) {
        ledc_channel_config(&ledc_channel_1[ch]);
    }



    // Initialize fade service.
    ledc_fade_func_install(0);

    while (1) {

        printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY_0);
        for (ch = 0; ch < LEDC_TEST_CH_NUM_0; ch++) {
            ledc_set_duty(ledc_channel_0[ch].speed_mode, ledc_channel_0[ch].channel, LEDC_TEST_DUTY_0);
            ledc_update_duty(ledc_channel_0[ch].speed_mode, ledc_channel_0[ch].channel);
        }
        printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY_1);
        for (ch = 0; ch < LEDC_TEST_CH_NUM_0; ch++) {
            ledc_set_duty(ledc_channel_1[ch].speed_mode, ledc_channel_1[ch].channel, LEDC_TEST_DUTY_1);
            ledc_update_duty(ledc_channel_1[ch].speed_mode, ledc_channel_1[ch].channel);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("4. LEDC set duty = 0 without fade\n");
        for (ch = 0; ch < LEDC_TEST_CH_NUM_1; ch++) {
            ledc_set_duty(ledc_channel_1[ch].speed_mode, ledc_channel_1[ch].channel, 0);
            ledc_update_duty(ledc_channel_1[ch].speed_mode, ledc_channel_1[ch].channel);
        }
        printf("4. LEDC set duty = 0 without fade\n");
        for (ch = 0; ch < LEDC_TEST_CH_NUM_0; ch++) {
            ledc_set_duty(ledc_channel_0[ch].speed_mode, ledc_channel_0[ch].channel, 0);
            ledc_update_duty(ledc_channel_0[ch].speed_mode, ledc_channel_0[ch].channel);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }
}
