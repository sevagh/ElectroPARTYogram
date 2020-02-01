
set(INCROOT ${PROJECT_SOURCE_DIR}/include/SFML/Audio)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SFML/Audio)

# all source files
set(SRC
    ${SRCROOT}/ALCheck.cpp
    ${SRCROOT}/ALCheck.hpp
    ${SRCROOT}/AlResource.cpp
    ${INCROOT}/AlResource.hpp
    ${SRCROOT}/AudioDevice.cpp
    ${SRCROOT}/AudioDevice.hpp
    ${INCROOT}/Export.hpp
    ${SRCROOT}/Listener.cpp
    ${INCROOT}/Listener.hpp
    ${SRCROOT}/Music.cpp
    ${INCROOT}/Music.hpp
    ${SRCROOT}/Sound.cpp
    ${INCROOT}/Sound.hpp
    ${SRCROOT}/SoundBuffer.cpp
    ${INCROOT}/SoundBuffer.hpp
    ${SRCROOT}/SoundBufferRecorder.cpp
    ${INCROOT}/SoundBufferRecorder.hpp
    ${SRCROOT}/InputSoundFile.cpp
    ${INCROOT}/InputSoundFile.hpp
    ${SRCROOT}/OutputSoundFile.cpp
    ${INCROOT}/OutputSoundFile.hpp
    ${SRCROOT}/SoundRecorder.cpp
    ${INCROOT}/SoundRecorder.hpp
    ${SRCROOT}/SoundSource.cpp
    ${INCROOT}/SoundSource.hpp
    ${SRCROOT}/SoundStream.cpp
    ${INCROOT}/SoundStream.hpp
)
source_group("" FILES ${SRC})

set(CODECS_SRC
    ${SRCROOT}/SoundFileFactory.cpp
    ${INCROOT}/SoundFileFactory.hpp
    ${INCROOT}/SoundFileFactory.inl
    ${INCROOT}/SoundFileReader.hpp
    ${SRCROOT}/SoundFileReaderFlac.hpp
    ${SRCROOT}/SoundFileReaderFlac.cpp
    ${SRCROOT}/SoundFileReaderOgg.hpp
    ${SRCROOT}/SoundFileReaderOgg.cpp
    ${SRCROOT}/SoundFileReaderWav.hpp
    ${SRCROOT}/SoundFileReaderWav.cpp
    ${INCROOT}/SoundFileWriter.hpp
    ${SRCROOT}/SoundFileWriterFlac.hpp
    ${SRCROOT}/SoundFileWriterFlac.cpp
    ${SRCROOT}/SoundFileWriterOgg.hpp
    ${SRCROOT}/SoundFileWriterOgg.cpp
    ${SRCROOT}/SoundFileWriterWav.hpp
    ${SRCROOT}/SoundFileWriterWav.cpp
)
source_group("codecs" FILES ${CODECS_SRC})

# let CMake know about our additional audio libraries paths (on Windows and OSX)
if(SFML_OS_WINDOWS)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/AL")
elseif(SFML_OS_MACOSX)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/extlibs/libs-osx/Frameworks")
elseif(SFML_OS_ANDROID)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/headers/AL")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/extlibs/android")
endif()

# find external libraries
#sfml_find_package(OpenAL INCLUDE "OPENAL_INCLUDE_DIR" LINK "OPENAL_LIBRARY")
#sfml_find_package(VORBIS INCLUDE "VORBIS_INCLUDE_DIRS" LINK "VORBIS_LIBRARIES")
#sfml_find_package(FLAC INCLUDE "FLAC_INCLUDE_DIR" LINK "FLAC_LIBRARY")

# avoids warnings in vorbisfile.h
#target_compile_definitions(VORBIS INTERFACE "OV_EXCLUDE_STATIC_CALLBACKS")
#target_compile_definitions(FLAC INTERFACE "FLAC__NO_DLL")

# define the sfml-audio target
sfml_add_library(sfml-audio
                 SOURCES ${SRC} ${CODECS_SRC})

# setup dependencies
target_link_libraries(sfml-audio PRIVATE OpenAL)

if(SFML_OS_ANDROID)
    target_link_libraries(sfml-audio PRIVATE android OpenSLES)
endif()

set(EXTRA_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cpp/thirdparty-libs/)

add_library(vorbis SHARED IMPORTED)
set_property(TARGET vorbis PROPERTY IMPORTED_LOCATION "${EXTRA_LIB_DIR}/libvorbis.a")

add_library(vorbisenc SHARED IMPORTED)
set_property(TARGET vorbisenc PROPERTY IMPORTED_LOCATION "${EXTRA_LIB_DIR}/libvorbisenc.a")

add_library(vorbisfile SHARED IMPORTED)
set_property(TARGET vorbisfile PROPERTY IMPORTED_LOCATION "${EXTRA_LIB_DIR}/libvorbisfile.a")

add_library(FLAC SHARED IMPORTED)
set_property(TARGET FLAC PROPERTY IMPORTED_LOCATION "${EXTRA_LIB_DIR}/libFLAC.a")

add_library(FLAC++ SHARED IMPORTED)
set_property(TARGET FLAC++ PROPERTY IMPORTED_LOCATION "${EXTRA_LIB_DIR}/libFLAC++.a")

add_library(ogg SHARED IMPORTED)
set_property(TARGET ogg PROPERTY IMPORTED_LOCATION "${EXTRA_LIB_DIR}/libogg.a")

target_link_libraries(sfml-audio
                      PUBLIC sfml-system
		      PRIVATE vorbisenc vorbisfile vorbis ogg FLAC FLAC++)
