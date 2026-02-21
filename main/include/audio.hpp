#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>

class Audio {
  public:
    Audio(uint32_t bclk_pin, uint32_t ws_pin, uint32_t data_pin,
          uint32_t sample_rate = 44100);
    ~Audio();

    void init();
    float computeRMS(const std::size_t num_samples) const;
    void hpf(float *buffer, const std::size_t N, const float cutoffHz = 20.0f) const;
    void lpf(float *buffer, const std::size_t N, const float cutoffHz = 8000.0f) const;
    void hann(float *buffer, const std::size_t N) const;
    std::pair<std::size_t, const float *> readSamples() const;

  private:
    uint32_t bclk_pin_;
    uint32_t ws_pin_;
    uint32_t data_pin_;
    uint32_t sample_rate_;
    int32_t *data_buffer_;
    float *dsp_buffer_;
    float dt;
    static constexpr float TAU = 2.0f * M_PIf32;
};
