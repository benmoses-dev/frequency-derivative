#include "audio.hpp"
#include "driver/i2s.h"
#include "driver/i2s_types.h"
#include "esp_log.h"
#include <cmath>
#include <cstring>

static const char *TAG = "AUDIO";

Audio::Audio(const DSP &dsp) : dsp_(dsp) {
    data_buffer_ = (std::int32_t *)heap_caps_malloc(buffer_size_ * sizeof(std::int32_t),
                                                    MALLOC_CAP_DMA);
    if (!data_buffer_) {
        ESP_LOGE(TAG, "Failed to allocate DMA buffer");
    }
    dsp_buffer_ = (float *)heap_caps_malloc(buffer_size_ * sizeof(float), MALLOC_CAP_DMA);
    if (!dsp_buffer_) {
        ESP_LOGE(TAG, "Failed to allocate DMA float buffer");
    }
}

Audio::~Audio() { i2s_driver_uninstall(I2S_NUM_0); }

bool Audio::init() {
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

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, nullptr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %d", err);
        return false;
    }
    const i2s_pin_config_t pinConfig = {.mck_io_num = I2S_PIN_NO_CHANGE,
                                        .bck_io_num = static_cast<int>(bclk_pin_),
                                        .ws_io_num = static_cast<int>(ws_pin_),
                                        .data_out_num = I2S_PIN_NO_CHANGE,
                                        .data_in_num = static_cast<int>(data_pin_)};

    err = i2s_set_pin(I2S_NUM_0, &pinConfig);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pin: %d", err);
        return false;
    }
    ESP_LOGI(TAG, "I2S initialised on BCLK=%d LRCLK=%d DATA=%d", bclk_pin_, ws_pin_,
             data_pin_);
    return true;
}

std::pair<std::size_t, const float *> Audio::readSamples() const {
    if (!data_buffer_ || buffer_size_ == 0) {
        return {0, nullptr};
    }
    std::size_t bytesRead = 0;
    const esp_err_t err =
        i2s_read(I2S_NUM_0, data_buffer_, buffer_size_ * sizeof(std::int32_t), &bytesRead,
                 portMAX_DELAY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2S read failed: %d", err);
        return {0, nullptr};
    }
    const std::size_t count = bytesRead / sizeof(std::int32_t);
    constexpr float INT24_MAX = 8388608.0f; // 2^23 to normalise to [-1, 1] for FFT
    for (std::size_t i = 0; i < count; i++) {
        dsp_buffer_[i] = static_cast<float>(data_buffer_[i] >> 8) / INT24_MAX;
    }
    dsp_.hpf(dsp_buffer_, count);
    dsp_.lpf(dsp_buffer_, count);
    dsp_.hann(dsp_buffer_, count);
    return {count, dsp_buffer_};
}
