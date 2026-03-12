#pragma once

#include "led_strip.h"
#include <cstdint>

#define LED_GPIO 18
#define LED_COUNT 24

typedef struct {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
} LED;

class LEDStrip {
  public:
    void init();
    void decay();
    void output(const float maxMag, const float t);
    void clear();

  private:
    void toRGB(float h, float v, std::uint8_t &red, std::uint8_t &green,
               std::uint8_t &blue);
    led_strip_handle_t ledStrip;
    LED buffer[LED_COUNT];
    static constexpr float fade = 0.8f;
    std::uint8_t idx = 0;
    std::uint8_t red = 0;
    std::uint8_t green = 0;
    std::uint8_t blue = 0;
};
