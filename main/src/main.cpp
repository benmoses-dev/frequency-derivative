#include "audio.hpp"
#include "esp_log.h"
#include "fft.hpp"
#include "led.hpp"
#include <algorithm>
#include <cmath>

#define SCK_PIN 26
#define WS_PIN 25
#define SD_PIN 33
#define SAMPLE_RATE 44100
#define topK 3

static const char *TAG = "MAIN";
static constexpr float maxFreq = 8'000.0f;
static constexpr float minFreq = 120.0f;
static constexpr float gain = 0.3f;
static constexpr float minMag = 0.1f;

struct Bin {
    float mag;
    float freq;
    bool operator<(const Bin &other) const { return mag < other.mag; }
    bool operator>(const Bin &other) const { return mag > other.mag; }
    bool operator==(const Bin &other) const { return mag == other.mag; }
};

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
        const float binHz = SAMPLE_RATE / static_cast<float>(N);

        float maxMag = 0.0f;
        std::vector<Bin> bins;
        bins.reserve((N / 2) - 2);
        /**
         * Skip DC, Nyquist.
         * Potentially also skip the first frequency bin (experiment).
         */
        for (std::size_t i = 2; i < N / 2; i++) {
            /**
             * Don't account for the FFT dot product for now
             * Account for the negative complex conjugate (optional)
             */
            const float mag = std::fabs(spectrum[i]) * 2.0f;
            /**
             * topK max-heap.
             */
            const float freq = i * binHz;
            bins.emplace_back(mag, freq);
            maxMag = std::max(maxMag, mag);
        }
        std::sort(bins.begin(), bins.end());

        float weightedSum = 0.0f;
        float normSum = 0.0f;
        float magSum = 0.0f;
        std::int32_t counted = 0;
        for (std::size_t i = bins.size() - 1; i > bins.size() - topK - 1; i--) {
            const auto &[mag, freq] = bins[i];
            /**
             * Experiment here - we may not want to normalise.
             * Also play around with the cutoff noise-floor
             */
            const float normMag = mag / maxMag;
            if (normMag < 0.2) {
                continue;
            }
            counted++;
            weightedSum += freq * normMag;
            normSum += normMag;
            magSum += mag;
        }

        float avgMag = counted > 0 ? (magSum / static_cast<float>(counted)) : 0.0f;
        const float C = (normSum > 0.0f) ? (weightedSum / normSum) : 0.0f;
        const float spectralCentroid = std::fmax(std::fmin(C, maxFreq), minFreq);
        const float numerator = std::log(spectralCentroid) - std::log(minFreq);
        const float denominator = std::log(maxFreq) - std::log(minFreq);
        const float t = std::fmin(numerator / denominator, 0.99f);
        if (avgMag < minMag) {
            avgMag = 0.0f;
        }
        float v = std::log1p(avgMag * gain);
        ESP_LOGI(TAG, "Counted: %d, Spectral centroid: %.2f Hz, avgMag: %.2f, hue: %.2f",
                 counted, spectralCentroid, avgMag, v);

        v = std::fmin(v, 1.0f);
        led.decay();
        led.output(v, t);
    }
}
