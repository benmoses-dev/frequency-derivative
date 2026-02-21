#include "audio.hpp"
#include "esp_log.h"
#include "fft.hpp"

#define SCK_PIN 26
#define WS_PIN 25
#define SD_PIN 33
#define SAMPLE_RATE 44100

extern "C" void app_main() {
    Audio audio(SCK_PIN, WS_PIN, SD_PIN, SAMPLE_RATE);
    audio.init();

    FTransform fft;

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

        const float binHZ = SAMPLE_RATE / static_cast<float>(N);
        const float dominantFreq = maxIndex * binHZ;
        ESP_LOGI("FFT", "Dominant frequency: %.1f Hz (mag %.2f)", dominantFreq, maxMag);
        // const double rms = audio.computeRMS(samples);
        // ESP_LOGI("MAIN", "RMS: %.2f", rms);
    }
}
