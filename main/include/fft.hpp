#pragma once

#include <cmath>
#include <complex>
#include <vector>

class FTransform {
  private:
    static constexpr float TAU = 2.0f * M_PI;

    void radix2Pad(std::vector<std::complex<float>> &in) const;

    void bitReverse(std::vector<std::complex<float>> &in) const;

    std::pair<std::complex<float>, std::complex<float>>
    applyTwiddle(const std::complex<float> &even, const std::complex<float> &odd,
                 const std::complex<float> &twiddle) const;

    void fftIt(std::vector<std::complex<float>> &in, const bool inverse) const;

    template <typename T>
    std::vector<std::complex<float>> normalise(const T *in, const std::size_t N) const {
        std::vector<std::complex<float>> res;
        res.reserve(N);
        using V = std::remove_cv_t<T>;
        if constexpr (std::is_same_v<V, std::complex<float>>) {
            for (std::size_t i = 0; i < N; i++) {
                res.emplace_back(in[i]);
            }
        } else {
            for (std::size_t i = 0; i < N; i++) {
                res.emplace_back(in[i], 0.0f);
            }
        }
        return res;
    }

    std::vector<std::complex<float>>
    fftInternal(std::vector<std::complex<float>> &spectrum, const bool inverse) const;

  public:
    explicit FTransform();

    std::vector<std::complex<float>> fft(const float *input, const std::size_t N,
                                         const bool inverse = false) const;

    std::vector<std::complex<float>> fft(const std::complex<float> *input,
                                         const std::size_t N,
                                         const bool inverse = false) const;
};
