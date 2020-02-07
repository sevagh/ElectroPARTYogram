# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.10)

include(${CMAKE_SOURCE_DIR}/cmake_modules/ClangFormat.cmake)

set(ANDROID_ABI arm64-v8a)

set(SRCS
		AudioEngine.cpp
		RecordingCallback.cpp
		BTrack.cpp
		OnsetDetection.cpp
		AndroidMain.cpp
		GraphicsLoop.cpp)

set(HDRS
		AudioEngine.h
		RecordingCallback.h
		BTrack.h
		OnsetDetection.h
		GraphicsLoop.h
		DrawParams.h
		BTrackPrecomputed.h
		Window.h)

add_library(
	electropartyogram
        SHARED
        ${SRCS})

clangformat_setup(
        ${SRCS} ${HDRS})

add_dependencies(electropartyogram clangformat)

find_library(
        log-lib
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

# for openal
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lOpenSLES -llog")
# compile openal-soft from source
set(OPENAL_DIR ./openal)
set(CMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
#set(LIBTYPE "STATIC")
add_subdirectory(${OPENAL_DIR} ./openal)
include_directories(${OPENAL_DIR}/include)

# SFML
set(CMAKE_IGNORE_PATH "") # dummy thing because of a stupid SFML/CMakeLists.txt
set(SFML_USE_SYSTEM_DEPS TRUE)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_ANDROID_NDK ${ANDROID_NDK})
set(CMAKE_ANDROID_ARCH_ABI aarch64)
set(CMAKE_ANDROID_STL_TYPE c++_static)
set(FREETYPE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/freetype2/libfreetype.a)
set(SFML_DIR ./SFML)
include_directories(${SFML_DIR}/extlibs/headers)
add_subdirectory(${SFML_DIR} ./SFML)
include_directories(${SFML_DIR}/include)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries(
		electropartyogram

        oboe
		NE10

		${log-lib}
        android

        EGL
        GLESv1_CM

		sfml-system
		OpenAL

		sfml-window
		sfml-audio
		sfml-graphics
		sfml-network
		sfml-activity
		-Wl,--whole-archive sfml-main -Wl,--no-whole-archive)
