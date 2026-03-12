#include "audio.hpp"
#include "esp_log.h"
#include "fft.hpp"
#include "led.hpp"

#define SCK_PIN 26
#define WS_PIN 25
#define SD_PIN 33
#define SAMPLE_RATE 44100

static const char *TAG = "MAIN";
static constexpr float maxFreq = 4'000.0f;
static constexpr float minFreq = 100.0f;
static constexpr float gain = 0.1f;
static constexpr float minMag = 0.1f;

extern "C" void app_main() {
    Audio audio(SCK_PIN, WS_PIN, SD_PIN, SAMPLE_RATE);
    audio.init();

    FTransform fft;

    LEDStrip led;
    led.init();

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

        const float numerator = std::log(dominantFreq) - std::log(minFreq);
        const float denominator = std::log(maxFreq) - std::log(minFreq);
        const float t = numerator / denominator;
        if (maxMag < minMag) {
            maxMag = 0.0f;
        }
        float v = std::log1p(maxMag * gain);
        v = std::fmin(v, 1.0f);
        led.decay();
        led.output(v, t);
    }
}
