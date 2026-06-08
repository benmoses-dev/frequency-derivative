idf_component_register(
    SRCS "src/main.cpp" "src/audio.cpp" "src/fft.cpp" "src/led.cpp" "src/dsp.cpp"
    INCLUDE_DIRS "include"
    REQUIRES driver led_strip
)
