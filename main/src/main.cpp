#include "audio.hpp"
#include "config.h"
#include "fft.hpp"
#include "led.hpp"
#include "esp_log.h"
#include <cmath>

static const char *TAG = "MAIN";

extern "C" void app_main() {
    ESP_LOGI(TAG, "Sample rate is %d", SAMPLE_RATE);
    ESP_LOGI(TAG, "Top K average is %d", TOP_K);
    ESP_LOGI(TAG, "FFT size is %d", BUFFER_SIZE);
    const DSP dsp(SAMPLE_RATE);
    Audio audio(dsp);
    if (!audio.init()) {
        return;
    };

    FTransform fft;

    LEDStrip led;
    if (!led.init()) {
        return;
    };

    while (true) {
        const auto [count, buff] = audio.readSamples();
        if (count == 0 || !buff) {
            continue;
        }
        const auto spectrum = fft.fft(buff, count);
        const auto [value, hue] = dsp.process(spectrum);
        led.decay();
        led.output(value, hue);
    }
}
