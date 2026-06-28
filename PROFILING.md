# Audio Processing Loop Profiling

This document summarises the performance of the audio processing loop on the ESP32, including time spent at each stage.

| Stage                         | Time (µs) | % of total loop |
|--------------------------------|------------|-----------------------------|
| Audio read | 14,820     | 69%                        |
| HPF + LPF + Hann | 1,450     | 7%                        |
| FFT                            | 3,140      | 15%                        |
| Average calculations / hue     | 830        | 4%                         |
| LED output                     | 1,100      | 5%                         |
| **Total**                      | **21,340** | 100%                       |

The processing latency is approximately ~6520µs, giving us ~14,820µs of headroom before the DMA buffers overflow.