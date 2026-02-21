#pragma once

#include <complex>
#include <vector>

class FTransform {
  private:
    static constexpr float PI = 3.14159265358979323846;
    static constexpr float TAU = 2 * PI;

    void radix2Pad(std::vector<std::complex<float>> &in) const;

    void bitReverse(std::vector<std::complex<float>> &in) const;

    std::pair<std::complex<float>, std::complex<float>>
    applyTwiddle(const std::complex<float> &even, const std::complex<float> &odd,
                 const std::complex<float> &twiddle) const;

    void fftIt(std::vector<std::complex<float>> &in, const bool inverse) const;

    std::vector<std::complex<float>> normalise(const float *in,
                                               const std::size_t N) const;

  public:
    explicit FTransform();

    std::vector<std::complex<float>> fft(const float *input, const std::size_t N,
                                         const bool inverse = false) const;
};
