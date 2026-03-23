#pragma once

#include "config.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>

class Audio {
  public:
    Audio();
    ~Audio();

    bool init();
    float computeRMS(const std::size_t num_samples) const;
    void hpf(float *buffer, const std::size_t N, const float cutoffHz = HPF_CUTOFF) const;
    void lpf(float *buffer, const std::size_t N, const float cutoffHz = LPF_CUTOFF) const;
    void hann(float *buffer, const std::size_t N) const;
    std::pair<std::size_t, const float *> readSamples() const;

  private:
    static constexpr std::uint32_t bclk_pin_ = SCK_PIN;
    static constexpr std::uint32_t ws_pin_ = WS_PIN;
    static constexpr std::uint32_t data_pin_ = SD_PIN;
    static constexpr std::uint32_t sample_rate_ = SAMPLE_RATE;
    static constexpr std::size_t buffer_size_ = BUFFER_SIZE;
    std::int32_t *data_buffer_;
    float *dsp_buffer_;
    float dt;
    static constexpr float TAU = 2.0f * M_PI;
};
