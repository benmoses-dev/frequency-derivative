#include "audio.hpp"
#include "config.h"
#include "driver/i2s.h"
#include "driver/i2s_types.h"
#include "esp_log.h"
#include "fft.hpp"
#include "led.hpp"

static const char *TAG = "MAIN";

extern "C" void app_main() {
    ESP_LOGI(TAG, "Sample rate is %d", SAMPLE_RATE);
    ESP_LOGI(TAG, "Top K average is %d", TOP_K);
    ESP_LOGI(TAG, "FFT size is %d", BUFFER_SIZE);

    const DSP dsp(SAMPLE_RATE);

    QueueHandle_t i2s_queue;
    Audio audio(dsp, i2s_queue);
    if (!audio.init()) {
        return;
    };

    FTransform fft;

    LEDStrip led;
    if (!led.init()) {
        return;
    };

    i2s_event_t event;
    while (true) {
#if DEBUG
        while (xQueueReceive(i2s_queue, &event, 0) == pdTRUE) {
            switch (event.type) {
            case I2S_EVENT_RX_Q_OVF:
                ESP_LOGE(TAG, "I2S RX overflow");
                break;
            case I2S_EVENT_DMA_ERROR:
                ESP_LOGE(TAG, "I2S DMA error");
                break;
            default:
                break;
            }
        }
#endif

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
