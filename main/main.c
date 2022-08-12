
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "tusb_hid.h"
#include "class/hid/hid.h"
#include "driver/gpio.h"
#include "descriptors_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "led_strip.h"

#include "config.h"
#include "keyboard.h"

#define TAG "HID"

static led_strip_handle_t led_strip;

// keycodes in components/tinyusb/tinyusb/src/class/hid/hid.h
#define MESSAGE "Ich kann schneller tippen als Du"

// check tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, _keycode); for mod keys

static void init_LED(void) {
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,
        .max_leds = LED_NUM,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

static void drive_LED(void* arg) {
    while(!tud_hid_ready()) {
        for (int i=0; i<LED_NUM; i++) {
            led_strip_set_pixel(led_strip, i, 0x40, 0x00, 0x00);
        }
        led_strip_refresh(led_strip);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    switch (PATTERN) {
        case PATTERN_OFF:
            while(1) {
                led_strip_clear(led_strip);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            break;
        case PATTERN_STATIC:
            uint32_t red = (PATTERN_COL & 0xFF0000) >> 16;
            uint32_t green = (PATTERN_COL & 0x00FF00) >> 8;
            uint32_t blue = (PATTERN_COL & 0x0000FF);
            while (1) {
                for (int i=0; i<LED_NUM; i++) {
                    led_strip_set_pixel(led_strip, i, red, green, blue);
                }
                led_strip_refresh(led_strip);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            break;
        default:
            break;
    }
}

bool is_upper(char c) {
    return (c >= 'A' && c <= 'Z');
}

bool is_lower(char c) {
    return (c >= 'a' && c <= 'z');
}

char to_lower(char c) {
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

uint32_t char_to_keycode(char c) {
    switch (c) {
        case 'a':
            return HID_KEY_A;
        case 'b':
            return HID_KEY_B;
        case 'c':
            return HID_KEY_C;
        case 'd':
            return HID_KEY_D;
        case 'e':
            return HID_KEY_E;
        case 'f':
            return HID_KEY_F;
        case 'g':
            return HID_KEY_G;
        case 'h':
            return HID_KEY_H;
        case 'i':
            return HID_KEY_I;
        case 'j':
            return HID_KEY_J;
        case 'k':
            return HID_KEY_K;
        case 'l':
            return HID_KEY_L;
        case 'm':
            return HID_KEY_M;
        case 'n':
            return HID_KEY_N;
        case 'o':
            return HID_KEY_O;
        case 'p':
            return HID_KEY_P;
        case 'q':
            return HID_KEY_Q;
        case 'r':
            return HID_KEY_R;
        case 's':
            return HID_KEY_S;
        case 't':
            return HID_KEY_T;
        case 'u':
            return HID_KEY_U;
        case 'v':
            return HID_KEY_V;
        case 'w':
            return HID_KEY_W;
        case 'x':
            return HID_KEY_X;
        case 'y':
            return HID_KEY_Y;
        case 'z':
            return HID_KEY_Z;
        case '1':
            return HID_KEY_1;
        case '2':
            return HID_KEY_2;
        case '3':
            return HID_KEY_3;
        case '4':
            return HID_KEY_4;
        case '5':
            return HID_KEY_5;
        case '6':
            return HID_KEY_6;
        case '7':
            return HID_KEY_7;
        case '8':
            return HID_KEY_8;
        case '9':
            return HID_KEY_9;
        case '0':
            return HID_KEY_0;
        case ' ':
            return HID_KEY_SPACE;
        default:
            return 0;
    }
}

void send_message(char* message, int len) {
    for (int i=0; i<len; i++) {
        if (is_upper(message[i])) {
            press_key(HID_KEY_SHIFT_LEFT);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            press_key(char_to_keycode(to_lower(message[i])));
            vTaskDelay(10 / portTICK_PERIOD_MS);
            release_key(char_to_keycode(to_lower(message[i])));
            vTaskDelay(10 / portTICK_PERIOD_MS);
            release_key(HID_KEY_SHIFT_LEFT);
        } else {
            press_key(char_to_keycode(message[i]));
            vTaskDelay(10 / portTICK_PERIOD_MS);
            release_key(char_to_keycode(message[i]));
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    press_key(HID_KEY_CONTROL_LEFT);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    press_key(HID_KEY_ENTER);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    release_key(HID_KEY_ENTER);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    release_key(HID_KEY_CONTROL_LEFT);
    
}

#if CONFIG_IDF_TARGET_ESP32S3
static void usb_otg_router_to_internal_phy()
{
    uint32_t *usb_phy_sel_reg = (uint32_t *)(0x60008000 + 0x120);
    *usb_phy_sel_reg |= BIT(19) | BIT(20);
}
#endif

void app_main(void)
{

#if CONFIG_IDF_TARGET_ESP32S3
    usb_otg_router_to_internal_phy();
#endif

    ESP_LOGI(TAG, "USB initialization");
    tinyusb_config_t tusb_cfg = {}; // the configuration using default values
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    ESP_LOGI(TAG, "LED initialization");
    init_LED();

    ESP_LOGI(TAG, "LED task start");
    TaskHandle_t xLedHandle = NULL;
    xTaskCreatePinnedToCore(drive_LED, "drive_LED", 3072, NULL, 1, &xLedHandle, 1);

    while (1) {
        if (!tud_hid_ready()) {
            ESP_LOGI(TAG, "USB not ready");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        uint8_t _keycode[6] = { 0 };

        vTaskDelay(100 / portTICK_PERIOD_MS);
        send_message(MESSAGE, strlen(MESSAGE));

        // ESP_LOGI(TAG, "pressing B");
        // _keycode[0] = HID_KEY_B;

        // tinyusb_hid_keyboard_report(_keycode);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
