#include "audio.hpp"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "AUDIO";

Audio::Audio(const DSP &d) : dsp(d) {
    dataBuffer = static_cast<std::int32_t *>(
        heap_caps_malloc(BUFFER_SIZE * sizeof(std::int32_t), MALLOC_CAP_DEFAULT));
    if (!dataBuffer) {
        ESP_LOGE(TAG, "Failed to allocate buffer");
    }
    dspBuffer = static_cast<float *>(
        heap_caps_malloc(BUFFER_SIZE * sizeof(float), MALLOC_CAP_DEFAULT));
    if (!dspBuffer) {
        ESP_LOGE(TAG, "Failed to allocate float buffer");
    }
}

Audio::~Audio() {
    i2s_channel_disable(rxHandle);
    i2s_del_channel(rxHandle);
    heap_caps_free(dataBuffer);
    heap_caps_free(dspBuffer);
}

static IRAM_ATTR bool overflowCallback(i2s_chan_handle_t handle, i2s_event_data_t *event,
                                       void *data) {
#if DEBUG
    ESP_LOGE(TAG, "rx buffer overflow!");
#endif
    return false;
}

bool Audio::init() {
#if DEBUG
    ESP_LOGI(TAG, "Initialising I2S...");
#endif

    /**
     * Using 4 256 DMA buffers here should be OK, as we are processing at ~10ms but only
     * filling 1024 samples every ~21ms (with a 48kHz sample rate). The benefit over a
     * larger buffer is less latency, the drawback is no headroom for overflow.
     */
    const uint32_t len = BUFFER_SIZE / 4;
#if DEBUG
    ESP_LOGI(TAG, "Buffer length: %zu", len);
#endif
    i2s_chan_config_t chanConfig = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4,
        .dma_frame_num = len,
        .auto_clear_after_cb = false,
        .auto_clear_before_cb = false,
        .allow_pd = false,
        .intr_priority = 0,
    };
    esp_err_t res = i2s_new_channel(&chanConfig, NULL, &rxHandle);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S rx channel: %d", res);
        return false;
    }

    i2s_std_config_t stdConfig = {
        .clk_cfg =
            {
                .sample_rate_hz = SAMPLE_RATE,
                .clk_src = I2S_CLK_SRC_APLL,
                .mclk_multiple = I2S_MCLK_MULTIPLE_256,
                .bclk_div = 8,
            },
        .slot_cfg =
            I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg =
            {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = static_cast<gpio_num_t>(BCLK_PIN),
                .ws = static_cast<gpio_num_t>(WS_PIN),
                .dout = I2S_GPIO_UNUSED,
                .din = static_cast<gpio_num_t>(DATA_PIN),
                .invert_flags =
                    {
                        .mclk_inv = false,
                        .bclk_inv = false,
                        .ws_inv = false,
                    },
            },
    };
    res = i2s_channel_init_std_mode(rxHandle, &stdConfig);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init I2S standard mode: %d", res);
        return false;
    }

    i2s_event_callbacks_t callbacks = {
        .on_recv = NULL,
        .on_recv_q_ovf = overflowCallback,
        .on_sent = NULL,
        .on_send_q_ovf = NULL,
    };
    i2s_channel_register_event_callback(rxHandle, &callbacks, NULL);

    i2s_channel_enable(rxHandle);

    ESP_LOGI(TAG, "I2S initialised on BCLK=%d LRCLK=%d DATA=%d", BCLK_PIN, WS_PIN,
             DATA_PIN);
    return true;
}

std::pair<std::size_t, const float *> Audio::readSamples() const {
    if (!dataBuffer || BUFFER_SIZE == 0) {
        return {0, nullptr};
    }

    std::size_t bytesRead = 0;
    const esp_err_t err =
        i2s_channel_read(rxHandle, dataBuffer, BUFFER_SIZE * sizeof(std::int32_t),
                         &bytesRead, portMAX_DELAY);
    if (err != ESP_OK) {
#if DEBUG
        ESP_LOGE(TAG, "I2S read failed: %d", err);
#endif
        return {0, nullptr};
    }

    const std::size_t count = bytesRead / sizeof(std::int32_t);
    constexpr float INT24_MAX = 8388608.0f; // 2^23 to normalise to [-1, 1] for FFT
    for (std::size_t i = 0; i < count; i++) {
        dspBuffer[i] = static_cast<float>(dataBuffer[i] >> 8) / INT24_MAX;
    }

    dsp.hpf(dspBuffer, count);
    dsp.lpf(dspBuffer, count);
    dsp.hann(dspBuffer, count);

    return {count, dspBuffer};
}
