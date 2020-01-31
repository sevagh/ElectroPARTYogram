# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10)

# app glue for a native activity
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library(app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(ANDROID_ABI arm64-v8a)


add_library( # Sets the name of the library.
        animals_as_meter

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        AudioEngine.cpp
        RecordingCallback.cpp
        BTrack.cpp
        OnsetDetection.cpp
        AndroidMain.cpp
        )

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

set(GCEM_DIR ./gcem)
include_directories(${GCEM_DIR}/include)

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

# compile freetype2 from source
set(FREETYPE_DIR ./freetype2)
set(CMAKE_BUILD_TYPE Release)
add_subdirectory(${FREETYPE_DIR} ./freetype2)
include_directories(${FREETYPE_DIR}/include)

# SFML for graphics
set(CMAKE_IGNORE_PATH "") # dummy thing because of a stupid SFML/CMakeLists.txt
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(SFML_USE_SYSTEM_DEPS TRUE)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_ANDROID_NDK ${ANDROID_NDK})
set(CMAKE_ANDROID_ARCH_ABI aarch64)
set(CMAKE_ANDROID_STL_TYPE c++_static)
set(FREETYPE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/freetype2/libfreetype.a)
set(SFML_DIR ./SFML)
add_subdirectory(${SFML_DIR} ./SFML)
include_directories(${SFML_DIR}/include )

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        animals_as_meter
        oboe
        android
        app-glue
        NE10
        freetype
        EGL
        GLESv1_CM
        sfml-system
        sfml-graphics

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})