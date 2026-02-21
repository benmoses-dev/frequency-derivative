idf_component_register(
    SRCS "src/main.cpp" "src/audio.cpp" "src/fft.cpp"
    INCLUDE_DIRS "include"
    REQUIRES driver
)
