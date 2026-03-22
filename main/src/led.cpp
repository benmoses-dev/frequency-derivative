#include "led.hpp"
#include "esp_log.h"
#include <cmath>

static const char *TAG = "LED";

void LEDStrip::toRGB(float h, float v, std::uint8_t &red, std::uint8_t &green,
                     std::uint8_t &blue) {
    const float hh = (h - std::floor(h)) * 6.0f;
    const float X = v * (1.0f - fabs(fmod(hh, 2.0f) - 1.0f));
    float r, g, b;
    const std::uint8_t sector = static_cast<std::uint8_t>(hh);
    switch (sector) {
    case 0:
        r = v;
        g = X;
        b = 0;
        break;
    case 1:
        r = X;
        g = v;
        b = 0;
        break;
    case 2:
        r = 0;
        g = v;
        b = X;
        break;
    case 3:
        r = 0;
        g = X;
        b = v;
        break;
    case 4:
        r = X;
        g = 0;
        b = v;
        break;
    default:
        r = v;
        g = 0;
        b = X;
        break;
    }
    red = static_cast<std::uint8_t>(r * 255.0f);
    green = static_cast<std::uint8_t>(g * 255.0f);
    blue = static_cast<std::uint8_t>(b * 255.0f);
}

void LEDStrip::init() {
    led_strip_config_t stripConfig = {};
    stripConfig.strip_gpio_num = LED_GPIO;
    stripConfig.max_leds = LED_COUNT;
    led_strip_rmt_config_t rmtConfig = {};
    rmtConfig.resolution_hz = LED_RESOLUTION;
    led_strip_new_rmt_device(&stripConfig, &rmtConfig, &ledStrip);
    led_strip_clear(ledStrip);
}

void LEDStrip::decay(const float fade) {
    for (int i = 0; i < LED_COUNT; i++) {
        buffer[i].value = (buffer[i].value * fade);
        std::uint8_t r, g, b;
        toRGB(buffer[i].hue, buffer[i].value, r, g, b);
        led_strip_set_pixel(ledStrip, i, r, g, b);
    }
}

void LEDStrip::output(const float brightness, const float frequency) {
    if (brightness > 0.0f) {
        idx = (idx + 1) % LED_COUNT;
    }
    buffer[idx].hue = frequency;
    buffer[idx].value = brightness;
    toRGB(frequency, brightness, red_, green_, blue_);
    // ESP_LOGI(TAG, "Red: %d, Green: %d, Blue: %d", red, green, blue);
    led_strip_set_pixel(ledStrip, idx, red_, green_, blue_);
    led_strip_refresh(ledStrip);
}

void LEDStrip::clear() {
    led_strip_clear(ledStrip);
    idx = 0;
}
