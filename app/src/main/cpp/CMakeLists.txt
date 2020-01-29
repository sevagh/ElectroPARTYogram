# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10)

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
        jni_bridge.cpp
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

# stompbox
set(STOMPBOX_DIR ./stompbox)
set(STOMPBOX_TESTS OFF CACHE BOOL "" FORCE)
#set(STOMPBOX_TESTS OFF)
add_subdirectory(${STOMPBOX_DIR} ./stompbox)
include_directories(${STOMPBOX_DIR}/include
        ${STOMPBOX_DIR}/src
        ${STOMPBOX_DIR}/thirdparty/Ne10/inc
        ${STOMPBOX_DIR}/thirdparty/gcem/include)

# Oboe
set(OBOE_DIR ./oboe)
add_subdirectory(${OBOE_DIR} ./oboe)
include_directories(${OBOE_DIR}/include)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        animals_as_meter
        oboe
        android
        stompbox

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})