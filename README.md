# ElectroPARTYogram

An electrocardiogram measures and displays a person's heartbeat. :zap: 🎉 ElectroPARTYogram is an Android app that uses real-time beat detection to create digital art synchronized to the rhythm, beats, and tempo of nearby music.

<img src=./doc/beat1.png width="300"> <img src=./doc/beat2.png width="300">

The BTrack code in ElectroPARTYogram takes 0.15ms on average to process a 1024-sample (~20ms @ 48000Hz Fs) frame, so its contribution to the perceptual latency is negligible. The optimized BTrack implementation in this codebase is approx. 2x faster than the original.

### Motivation

ElectroPARTYogram could be interesting as:

* A demonstration of an optimized implementation of [BTrack](https://github.com/adamstark/BTrack), which works well with live music on a budget Android phone
* A complete example of a modern native Android app using the latest NDK, aarch64/arm64-v8a and NEON SIMD extensions
* A project that combines Oboe and Ne10, which might be the building blocks of a modern low-latency audio DSP app for Android
* Building and compiling SFML to have access to a huge existing body of tutorials and examples

ElectroPARTYogram is a modern incarnation of my oldest side project, [Pitcha](https://github.com/sevagh/Pitcha), a 2015 Android real-time pitch tracking app. The projects share more similarities than differences:

1. Open a real-time stream of input audio data
2. Perform some DSP algorithm on it (in the 2015 one, pitch tracking - in this one, beat tracking)
3. Convert the result of the algorithm to a visual/graphical output
4. Display the results in real-time

### Download

You can download signed arm64-v8a/aarch64 APKs from [the releases page](https://github.com/sevagh/ElectroPARTYogram/releases). I have no app store plans yet, given the experimental nature of this code and my hesitance to provide long-term support.

### Development

ElectroPARTYogram is a mostly native (C++ - some Kotlin for record permission requesting) Android app targeting NDK r20.1.5948944 and an API level >= 24. The only ABI built is arm64-v8a for NEON support. All development and testing was done on my Nokia 6.1. Android Studio (version 3.5.3) was used for all development.

Dependencies are added as git submodules under app/src/main/cpp, and you'll most likely have to run `git submodule update --init --recursive` after cloning the project. I've included the SFML sources directly since I had to make tweaks to get it to compile, and the OpenAL sources from the tarball. Also, for ogg, vorbis, and FLAC I compiled static arm64-v8a libraries using the NDK as such:

```
sevagh:ogg $ mkdir -p build && cd build && \
        cmake .. \
            -DCMAKE_TOOLCHAIN_FILE=$HOME/Android/Sdk/ndk/20.1.5948944/build/cmake/android.toolchain.cmake \
            -DANDROID_NATIVE_API_LEVEL=24 \
            -DANDROID_ABI=arm64-v8a
```

The libraries are included [in this project](./app/src/main/cpp/thirdparty-libs) which sfml-audio is compiled against. You may download these to help your arm64-v8a SFML build efforts. Most of these details are in [the CMakeLists.txt](./app/src/main/cpp/CMakeLists.txt). Thanks to [realtime-beat-tracking](https://github.com/shortstheory/realtime-beat-tracking) as an example of SFML beat-based art, [SFML_AndroidStudio](https://github.com/Alia5/SFML_AndroidStudio) for SFML build-related details, and [oboe_recorder_sample](https://github.com/sheraz-nadeem/oboe_recorder_sample) as a good Oboe project template.
