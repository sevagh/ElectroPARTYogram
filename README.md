# ElectroPARTYogram

An electrocardiogram measures and displays a person's heartbeat. :zap: 🎉 ElectroPARTYogram is an Android app that uses real-time beat detection to create digital art synchronized to the rhythm and tempo of nearby music.

![screenshots](./screenshots_here.png)

ElectroPARTYgram could be interesting as:

* A demonstration of an optimized implementation of the robust real-time beat detection algorithm [BTrack](https://github.com/adamstark/BTrack), which works well with live music on a budget Android phone
* A complete example of a modern native Android app using the latest NDK, aarch64/arm64-v8a and NEON SIMD extensions
* An Oboe + Ne10 FFTs project, which might be the building blocks of a modern low-latency audio app for Android
* Building and compiling SFML to have access to a huge existing body of tutorials and examples

#### Hacking

ElectroPARTYogram is a mostly native (C++ - some Kotlin for record permission requesting) Android app targeting NDK r20.1.5948944 and an API level >= 24. The only ABI built is arm64-v8a for NEON support. All development and testing was done on my Nokia 6.1. Android Studio (version 3.5.3) was used for all development.

Dependencies are added as git submodules under app/src/main/cpp, and you'll most likely have to run `git submodule update --init --recursive` after cloning the project. I've included the SFML sources directly since I had to make tweaks to get it to compile. Also, for ogg, vorbis, and FLAC I compiled static arm64-v8a libraries using the NDK as such:

```
sevagh:ogg $ mkdir -p build && cd build && \
        cmake .. \
            -DCMAKE_TOOLCHAIN_FILE=$HOME/Android/Sdk/ndk/20.1.5948944/build/cmake/android.toolchain.cmake \
            -DANDROID_NATIVE_API_LEVEL=24 \
            -DANDROID_ABI=arm64-v8a
```

The libraries are included [in this project](./app/src/main/cpp/thirdparty-libs) which sfml-audio is compiled against. You may download these to help your arm64-v8a SFML build efforts. Most of these details are in [the CMakeLists.txt](./app/src/main/cpp/CMakeLists.txt). Thanks to [realtime-beat-tracking](https://github.com/shortstheory/realtime-beat-tracking) as an example of SFML beat-based art, [SFML_AndroidStudio](https://github.com/Alia5/SFML_AndroidStudio) for SFML build-related details, and [oboe_recorder_sample](https://github.com/sheraz-nadeem/oboe_recorder_sample) as a good Oboe project template.
