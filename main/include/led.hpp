#pragma once

#include "config.h"
#include "led_strip.h"
#include <cstdint>

typedef struct {
    float hue;
    float value;
} LED;

class LEDStrip {
  public:
    bool init();
    void decay(const float fade = LED_FADE);
    void output(const float brightness, const float frequency);
    void clear();

  private:
    void toRGB(float h, float v, std::uint8_t &red, std::uint8_t &green,
               std::uint8_t &blue);
    led_strip_handle_t ledStrip;
    LED buffer[LED_COUNT];
    std::uint8_t idx = 0;
    std::uint8_t red_ = 0;
    std::uint8_t green_ = 0;
    std::uint8_t blue_ = 0;
};
