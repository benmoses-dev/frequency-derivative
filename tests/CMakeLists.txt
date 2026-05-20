cmake_minimum_required(VERSION 3.16)

project(fd_tests LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(MAIN_SRC ${CMAKE_CURRENT_LIST_DIR}/../main)

find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "Using system Google Test")
else()
    message(STATUS "System Google Test not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
enable_testing()
include(GoogleTest)

add_executable(fft_test fft_test.cpp ${MAIN_SRC}/src/fft.cpp)
target_include_directories(fft_test PRIVATE ${MAIN_SRC}/include)
target_link_libraries(fft_test PRIVATE GTest::gtest_main)
gtest_discover_tests(fft_test)
target_compile_options(fft_test PRIVATE
-O2 -g3
-fno-omit-frame-pointer -fsanitize=address,undefined
-Wall -Wextra -Wshadow -Wconversion -Wfloat-equal -Wpedantic -Werror=return-type)
target_link_options(fft_test PRIVATE -fsanitize=address,undefined)

add_executable(fft_bench fft_bench.cpp ${MAIN_SRC}/src/fft.cpp)
target_include_directories(fft_bench PRIVATE ${MAIN_SRC}/include)
target_compile_options(fft_bench PRIVATE 
-O2 -g3
-fno-omit-frame-pointer
-Wall -Wextra -Wshadow -Wconversion -Wfloat-equal -Wpedantic -Werror=return-type)

add_executable(dsp_test dsp_test.cpp ${MAIN_SRC}/src/dsp.cpp)
target_include_directories(dsp_test PRIVATE ${MAIN_SRC}/include)
target_link_libraries(dsp_test PRIVATE GTest::gtest_main)
gtest_discover_tests(dsp_test)
target_compile_options(dsp_test PRIVATE 
-O2 -g3
-fno-omit-frame-pointer -fsanitize=address,undefined
-Wall -Wextra -Wshadow -Wconversion -Wfloat-equal -Wpedantic -Werror=return-type)
target_link_options(dsp_test PRIVATE -fsanitize=address,undefined)
