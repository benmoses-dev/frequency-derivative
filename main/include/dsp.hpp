#pragma once

#include "config.h"
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <vector>

struct Bin {
    float mag;
    float freq;
    bool operator<(const Bin &other) const { return mag < other.mag; }
    bool operator>(const Bin &other) const { return mag > other.mag; }
    bool operator==(const Bin &other) const { return mag < other.mag + 1e-9 && mag > other.mag - 1e-9; }
};

class DSP {
  public:
    DSP(const std::uint32_t sample_rate_);
    ~DSP();
    float computeRMS(float *buffer, const std::size_t num_samples) const;
    void hpf(float *buffer, const std::size_t N, const float cutoffHz = HPF_CUTOFF) const;
    void lpf(float *buffer, const std::size_t N, const float cutoffHz = LPF_CUTOFF) const;
    void hann(float *buffer, const std::size_t N) const;
    std::pair<float, float>
    process(const std::vector<std::complex<float>> &spectrum) const;

  private:
    static constexpr float TAU = 2.0f * static_cast<float>(M_PI);
    const float dt_;
};
