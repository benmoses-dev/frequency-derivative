#pragma once

#include "config.h"
#include "driver/i2s.h"
#include "driver/i2s_types.h"
#include "dsp.hpp"
#include <cstddef>
#include <cstdint>

class Audio {
  public:
    explicit Audio(const DSP &d);
    ~Audio();
    Audio(const Audio &) = delete;
    Audio(const Audio &&) = delete;
    Audio operator=(const Audio &) = delete;
    Audio operator=(const Audio &&) = delete;

    bool init();
    std::pair<std::size_t, const float *> readSamples() const;

  private:
    static constexpr std::uint32_t BCLK_PIN = SCK_PIN;
    static constexpr std::uint32_t DATA_PIN = SD_PIN;
    std::int32_t *dataBuffer;
    float *dspBuffer;
    const DSP &dsp;
    i2s_chan_handle_t rxHandle;
};
