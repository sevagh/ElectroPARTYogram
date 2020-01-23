# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(ANDROID_ABI arm64-v8a)

#set(USE_KISS_FFT yes)
add_definitions(-DUSE_KISS_FFT)
set(WRAPPER_DIR ./vulkan_wrapper)
set(BTRACK_DIR ./BTrack)

add_library( # Sets the name of the library.
        animals_as_meter

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        SoundRecording.cpp
        AudioEngine.cpp
        RecordingCallback.cpp
        VulkanMain.cpp
        AndroidMain.cpp
        BeatTracker.cpp
        MathNeon.cpp
        MathNeonLog10.cpp
        OnsetDetection.cpp
        ${BTRACK_DIR}/src/BTrack.cpp
        ${BTRACK_DIR}/src/OnsetDetectionFunction.cpp
        ${BTRACK_DIR}/libs/kiss_fft130/kiss_fft.c
        ${WRAPPER_DIR}/vulkan_wrapper.cpp
        )

include_directories(${WRAPPER_DIR} ${BTRACK_DIR}/src ${BTRACK_DIR}/libs/kiss_fft130/)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Oboe
set(OBOE_DIR ./oboe)
add_subdirectory(${OBOE_DIR} ./oboe)
include_directories(${OBOE_DIR}/include)

# Ne10 for FFT
set(CMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
set(NE10_ANDROID_TARGET_ARCH "aarch64")
set(ANDROID_API_LEVEL 24)
set(NE10_DIR ./Ne10)
add_subdirectory(${NE10_DIR} ./Ne10)
include_directories(${NE10_DIR}/inc)

# libsamplerate for BTrack
set(LIBSR_DIR ./libsamplerate)
add_subdirectory(${LIBSR_DIR} ./libsamplerate)
include_directories(${LIBSR_DIR}/src)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DVK_USE_PLATFORM_ANDROID_KHR")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries( # Specifies the target library.
        animals_as_meter
        oboe
        NE10
        app-glue
        android
        samplerate

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})