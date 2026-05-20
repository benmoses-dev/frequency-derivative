#pragma once

#include "config.h"
#include "dsp.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>

class Audio {
  public:
    Audio(const DSP &dsp);
    ~Audio();

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
};
