# Animals-as-Meter

Animals-as-Meter targets Android API version >=24 for [Vulkan](https://developer.android.com/ndk/guides/graphics/getting-started.html) and [Oboe](https://github.com/google/oboe), and [NDK >=r21](https://developer.android.com/ndk/guides/cpu-arm-neon) for NEON SIMD instruction support and OpenMP. The only ABI targeted is aarch64/ARM v8 for default NEON support. All development was done on my Nokia 6.1.

Android Studio is used for all development.

### C++ dependencies

Dependencies are checked out as git submodules under app/src/main/cpp:

* [oboe](https://github.com/google/oboe) (release 1.3-stable)
* Ne10 (fill this)
