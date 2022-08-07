
#include <stdint.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "tusb_hid.h"
#include "class/hid/hid.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#define TAG "HID"

// keycodes in components/tinyusb/tinyusb/src/class/hid/hid.h

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

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

    while (1) {
        vTaskDelay(500 / portTICK_RATE_MS);
        uint8_t _keycode[6] = { 0 };

        ESP_LOGI(TAG, "pressing A");
        _keycode[0] = HID_KEY_A;

        tinyusb_hid_keyboard_report(_keycode);
    }
}
