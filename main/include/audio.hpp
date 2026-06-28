#pragma once

#include "config.h"
#include "driver/i2s.h"
#include "driver/i2s_types.h"
#include "dsp.hpp"
#include <cstddef>
#include <cstdint>

class Audio {
  public:
    explicit Audio(const DSP &dsp, QueueHandle_t &q);
    ~Audio();
    Audio(const Audio &) = delete;
    Audio(const Audio &&) = delete;
    Audio operator=(const Audio &) = delete;
    Audio operator=(const Audio &&) = delete;

    bool init();
    std::pair<std::size_t, const float *> readSamples() const;

  private:
    static constexpr std::uint32_t bclk_pin_ = SCK_PIN;
    static constexpr std::uint32_t ws_pin_ = WS_PIN;
    static constexpr std::uint32_t data_pin_ = SD_PIN;
    static constexpr std::uint32_t sample_rate_ = SAMPLE_RATE;
    static constexpr std::size_t buffer_size_ = BUFFER_SIZE;
    std::int32_t *data_buffer_;
    float *dsp_buffer_;
    const DSP &dsp_;
    QueueHandle_t &i2s_queue;
};
