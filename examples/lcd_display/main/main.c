#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/esp32_s3_eye.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "iot_button.h"
#include "lvgl.h"

static const char *TAG = "app";

#ifndef FW_VERSION
#define FW_VERSION "unknown"
#endif

static lv_obj_t    *s_capture_img  = NULL;
static lv_img_dsc_t s_img_dsc      = {0};
static uint8_t     *s_img_buf      = NULL;
static size_t       s_img_buf_size = 0;

/* ---------- Display ---------- */
static void app_lcd_init(void) {
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size   = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .flags         = {.buff_spiram = true},
    };
    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();

    s_capture_img = lv_img_create(lv_scr_act());
    lv_obj_set_size(s_capture_img, BSP_LCD_H_RES, BSP_LCD_V_RES);
    lv_obj_center(s_capture_img);
    lv_obj_add_flag(s_capture_img, LV_OBJ_FLAG_HIDDEN);

    ESP_LOGI(TAG, "LCD ready (%dx%d)", BSP_LCD_H_RES, BSP_LCD_V_RES);
}

/* ---------- Button ---------- */
static void on_button_press(void *button_handle, void *usr_data) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        return;
    }
    ESP_LOGI(TAG, "Captured %dx%d, %zu bytes", fb->width, fb->height, fb->len);

    /* Grow the persistent buffer if needed (first capture or resolution change) */
    if (fb->len > s_img_buf_size) {
        free(s_img_buf);
        s_img_buf      = malloc(fb->len);
        s_img_buf_size = s_img_buf ? fb->len : 0;
    }

    if (s_img_buf) {
        memcpy(s_img_buf, fb->buf, fb->len);

        /* Camera outputs big-endian RGB565; LVGL v9 stores little-endian.
         * Swap byte pairs to fix the color distortion. */
        for (size_t i = 0; i < fb->len; i += 2) {
            uint8_t tmp      = s_img_buf[i];
            s_img_buf[i]     = s_img_buf[i + 1];
            s_img_buf[i + 1] = tmp;
        }

        s_img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
        s_img_dsc.header.w  = fb->width;
        s_img_dsc.header.h  = fb->height;
        s_img_dsc.data_size = fb->len;
        s_img_dsc.data      = s_img_buf;

        if (bsp_display_lock(0)) {
            lv_img_set_src(s_capture_img, &s_img_dsc);
            lv_obj_clear_flag(s_capture_img, LV_OBJ_FLAG_HIDDEN);
            bsp_display_unlock();
        }
    }

    esp_camera_fb_return(fb);
}

static void app_button_init(void) {
    button_handle_t btns[BSP_BUTTON_NUM];
    ESP_ERROR_CHECK(bsp_iot_button_create(btns, NULL, BSP_BUTTON_NUM));
    iot_button_register_cb(btns[BSP_BUTTON_MENU], BUTTON_PRESS_DOWN, NULL, on_button_press, NULL);
    ESP_LOGI(TAG, "Button MENU ready");
}

/* ---------- Main ---------- */
void app_main(void) {
    ESP_LOGI(TAG, "=== ESP32-S3-EYE: button-capture demo ===");
    ESP_LOGI(TAG, "Firmware version: %s", FW_VERSION);

    /* I2C must be initialized before the camera */
    ESP_ERROR_CHECK(bsp_i2c_init());

    /* Initialize display */
    app_lcd_init();

    /* Initialize camera */
    camera_config_t cam_cfg = BSP_CAMERA_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(esp_camera_init(&cam_cfg));
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, BSP_CAMERA_VFLIP);
    s->set_hmirror(s, BSP_CAMERA_HMIRROR);
    ESP_LOGI(TAG, "Camera ready");

    /* Initialize button */
    app_button_init();

    ESP_LOGI(TAG, "Ready. Press MENU to capture.");

    /* LVGL is handled by a dedicated task created in bsp_display_start_with_config().
     * Just yield to let other tasks (button, LVGL, IDLE) run. */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
