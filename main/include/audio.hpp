#pragma once

#include <cstddef>
#include <cstdint>

class Audio {
  public:
    Audio(uint32_t bclk_pin, uint32_t ws_pin, uint32_t data_pin,
          uint32_t sample_rate = 44100);
    ~Audio();

    void init();
    size_t readSamples() const;
    double computeRMS(const size_t num_samples) const;

  private:
    uint32_t bclk_pin_;
    uint32_t ws_pin_;
    uint32_t data_pin_;
    uint32_t sample_rate_;
    int32_t *data_buffer_;
};
