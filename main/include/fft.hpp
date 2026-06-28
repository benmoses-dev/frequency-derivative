#pragma once

#include <complex>
#include <vector>

class FTransform {
  private:
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

  public:
    explicit FTransform();

    std::vector<std::complex<float>> fft(const float *input, const std::size_t N,
                                         const bool inverse = false) const;

    std::vector<std::complex<float>> fft(const std::complex<float> *input,
                                         const std::size_t N,
                                         const bool inverse = false) const;
};
