#include "audio.hpp"
#include "esp_log.h"
#include "fft.hpp"
#include "led_strip.h"

#define SCK_PIN 26
#define WS_PIN 25
#define SD_PIN 33
#define SAMPLE_RATE 44100
#define LED_GPIO 18
#define LED_COUNT 24

led_strip_handle_t ledStrip;
static constexpr float brightness = 0.2f;
static constexpr float maxFreq = 2'000.0f;
static constexpr float minFreq = 800.0f;
static constexpr float freqRange = maxFreq - minFreq;
static constexpr float fade = 0.8f;
static const char *TAG = "MAIN";

typedef struct {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
} LED;

LED buffer[LED_COUNT];

void toRGB(float h, std::uint8_t &red, std::uint8_t &green, std::uint8_t &blue) {
    const float s = 1.0f;
    const float v = brightness;
    h = h - std::floor(h);
    const float hh = h * 6.0f;
    const std::int32_t i = static_cast<std::int32_t>(hh);
    const float ff = hh - i;
    const float p = v * (1.0f - s);
    const float q = v * (1.0f - s * ff);
    const float t = v * (1.0f - s * (1.0f - ff));
    switch (i) {
    case 0:
        red = static_cast<std::uint8_t>(v * 255);
        green = static_cast<std::uint8_t>(t * 255);
        blue = static_cast<std::uint8_t>(p * 255);
        break;
    case 1:
        red = static_cast<std::uint8_t>(q * 255);
        green = static_cast<std::uint8_t>(v * 255);
        blue = static_cast<std::uint8_t>(p * 255);
        break;
    case 2:
        red = static_cast<std::uint8_t>(p * 255);
        green = static_cast<std::uint8_t>(v * 255);
        blue = static_cast<std::uint8_t>(t * 255);
        break;
    case 3:
        red = static_cast<std::uint8_t>(p * 255);
        green = static_cast<std::uint8_t>(q * 255);
        blue = static_cast<std::uint8_t>(v * 255);
        break;
    case 4:
        red = static_cast<std::uint8_t>(t * 255);
        green = static_cast<std::uint8_t>(p * 255);
        blue = static_cast<std::uint8_t>(v * 255);
        break;
    default:
        red = static_cast<std::uint8_t>(v * 255);
        green = static_cast<std::uint8_t>(p * 255);
        blue = static_cast<std::uint8_t>(q * 255);
        break;
    }
}

extern "C" void app_main() {
    Audio audio(SCK_PIN, WS_PIN, SD_PIN, SAMPLE_RATE);
    audio.init();

    FTransform fft;

    led_strip_config_t stripConfig = {};
    stripConfig.strip_gpio_num = LED_GPIO;
    stripConfig.max_leds = LED_COUNT;

    led_strip_rmt_config_t rmtConfig = {};
    rmtConfig.resolution_hz = 10 * 1000 * 1000;

    led_strip_new_rmt_device(&stripConfig, &rmtConfig, &ledStrip);
    led_strip_clear(ledStrip);

    std::uint8_t red = 0;
    std::uint8_t green = 0;
    std::uint8_t blue = 0;
    std::uint8_t idx = 0;

    while (true) {
        const auto [count, buff] = audio.readSamples();
        if (count == 0 || !buff) {
            continue;
        }

        const auto spectrum = fft.fft(buff, count);
        const std::size_t N = spectrum.size();

        float maxMag = 0.0f;
        std::size_t maxIndex = 0;

        for (std::size_t i = 2; i < N / 2; i++) {
            const float mag = std::abs(spectrum[i]);
            if (mag > maxMag) {
                maxMag = mag;
                maxIndex = i;
            }
        }

        const float binHz = SAMPLE_RATE / static_cast<float>(N);
        const float dominantFreq =
            std::fmax(std::fmin(maxIndex * binHz, maxFreq), minFreq);
        ESP_LOGI(TAG, "Dominant frequency: %.1f Hz (mag %.2f)", dominantFreq, maxMag);
        idx = (idx + 1) % 24;
        for (int i = 0; i < LED_COUNT; i++) {
            buffer[i].r = static_cast<std::uint8_t>(buffer[i].r * fade);
            buffer[i].g = static_cast<std::uint8_t>(buffer[i].g * fade);
            buffer[i].b = static_cast<std::uint8_t>(buffer[i].b * fade);
            led_strip_set_pixel(ledStrip, i, buffer[i].r, buffer[i].g, buffer[i].b);
        }
        if (maxMag > 0.1f) {
            const float t = (dominantFreq - minFreq) / freqRange;
            toRGB(t, red, green, blue);
            ESP_LOGI(TAG, "Red: %d, Green: %d, Blue: %d", red, green, blue);
        }
        buffer[idx].r = red;
        buffer[idx].g = green;
        buffer[idx].b = blue;
        led_strip_set_pixel(ledStrip, idx, red, green, blue);
        led_strip_refresh(ledStrip);
    }
}
