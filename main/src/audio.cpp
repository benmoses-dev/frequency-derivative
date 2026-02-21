#include "audio.hpp"
#include "driver/i2s.h"
#include "driver/i2s_types.h"
#include "esp_log.h"
#include <cmath>
#include <cstring>

#define BUFFER_SIZE 1024

static const char *TAG = "AUDIO";

Audio::Audio(const uint32_t bclk_pin, const uint32_t ws_pin, const uint32_t data_pin,
             const uint32_t sample_rate)
    : bclk_pin_(bclk_pin), ws_pin_(ws_pin), data_pin_(data_pin),
      sample_rate_(sample_rate) {
    data_buffer_ =
        (int32_t *)heap_caps_malloc(BUFFER_SIZE * sizeof(int32_t), MALLOC_CAP_DMA);
    if (!data_buffer_) {
        ESP_LOGE("AUDIO", "Failed to allocate DMA buffer");
    }
    dsp_buffer_ = (float *)heap_caps_malloc(BUFFER_SIZE * sizeof(float), MALLOC_CAP_DMA);
    if (!dsp_buffer_) {
        ESP_LOGE("AUDIO", "Failed to allocate DMA float buffer");
    }
}

Audio::~Audio() { i2s_driver_uninstall(I2S_NUM_0); }

void Audio::init() {
    ESP_LOGI(TAG, "Initialising I2S...");
    const i2s_config_t i2sConfig = {.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
                                    .sample_rate = sample_rate_,
                                    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
                                    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
                                    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
                                    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
                                    .dma_buf_count = 8,
                                    .dma_buf_len = 256,
                                    .use_apll = true,
                                    .tx_desc_auto_clear = true,
                                    .fixed_mclk = 0,
                                    .mclk_multiple = I2S_MCLK_MULTIPLE_128,
                                    .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT};

    const esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, nullptr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %d", err);
        return;
    }
    const i2s_pin_config_t pinConfig = {.mck_io_num = I2S_PIN_NO_CHANGE,
                                        .bck_io_num = static_cast<int>(bclk_pin_),
                                        .ws_io_num = static_cast<int>(ws_pin_),
                                        .data_out_num = I2S_PIN_NO_CHANGE,
                                        .data_in_num = static_cast<int>(data_pin_)};

    i2s_set_pin(I2S_NUM_0, &pinConfig);
    ESP_LOGI(TAG, "I2S initialised on BCLK=%d LRCLK=%d DATA=%d", bclk_pin_, ws_pin_,
             data_pin_);
}

float Audio::computeRMS(const size_t numSamples) const {
    if (!data_buffer_ || numSamples == 0) {
        return 0.0;
    }
    float sum = 0;
    for (std::size_t i = 0; i < numSamples; i++) {
        const float sample = data_buffer_[i] >> 8; // 24-bit mic in 32-bit int
        sum += sample * sample;
    }
    return sqrt(sum / numSamples);
}

void Audio::highPassFilter(float *buffer, const std::size_t N,
                           const float cutoffHz) const {
    if (N == 0) {
        return;
    }
    const float RC = 1.0f / (2.0f * M_PI * cutoffHz);
    const float dt = 1.0f / static_cast<float>(sample_rate_);
    const float alpha = RC / (RC + dt);
    float prevX = buffer[0];
    float prevY = 0.0f;
    for (std::size_t i = 0; i < N; i++) {
        const float x = buffer[i];
        const float y = alpha * (prevY + x - prevX);
        buffer[i] = y;
        prevX = x;
        prevY = y;
    }
}

void Audio::lowPassFilter(float *buffer, const std::size_t N,
                          const float cutoffHz) const {
    if (N == 0) {
        return;
    }
    const float RC = 1.0f / (2.0f * M_PI * cutoffHz);
    const float dt = 1.0f / static_cast<float>(sample_rate_);
    const float alpha = dt / (RC + dt);
    float prevY = buffer[0];
    for (std::size_t i = 0; i < N; i++) {
        const float x = buffer[i];
        const float y = alpha * x + (1.0f - alpha) * prevY;
        buffer[i] = y;
        prevY = y;
    }
}

void Audio::applyHannWindow(float *buffer, const std::size_t N) const {
    for (std::size_t i = 0; i < N; i++) {
        const float w = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (N - 1)));
        buffer[i] *= w;
    }
}

std::pair<size_t, const float *> Audio::readSamples() const {
    if (!data_buffer_ || BUFFER_SIZE == 0) {
        return {0, nullptr};
    }
    size_t bytesRead = 0;
    const esp_err_t err = i2s_read(I2S_NUM_0, data_buffer_, BUFFER_SIZE * sizeof(int32_t),
                                   &bytesRead, portMAX_DELAY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2S read failed: %d", err);
        return {0, nullptr};
    }

    const std::size_t count = bytesRead / sizeof(int32_t);

    // for (std::size_t i = 0; i < count; i++) {
    //     dsp_buffer_[i] = static_cast<float>(data_buffer_[i]);
    // }

    constexpr float INT24_MAX = 8388608.0f; // 2^23
    for (std::size_t i = 0; i < count; i++) {
        dsp_buffer_[i] = static_cast<float>(data_buffer_[i] >> 8) / INT24_MAX;
    }

    highPassFilter(dsp_buffer_, count);
    lowPassFilter(dsp_buffer_, count);
    applyHannWindow(dsp_buffer_, count);

    return {count, dsp_buffer_};
}
