#include "audio.hpp"
#include "esp_log.h"

#define SCK_PIN 26
#define WS_PIN 25
#define SD_PIN 33
#define SAMPLE_RATE 44100

extern "C" void app_main() {
    Audio audio(SCK_PIN, WS_PIN, SD_PIN, SAMPLE_RATE);
    audio.init();

    while (true) {
        const size_t samples = audio.readSamples();
        const double rms = audio.computeRMS(samples);
        ESP_LOGI("MAIN", "RMS: %.2f", rms);
    }
}
