#include "AudioEngine.h"
#include "NE10.h"
#include <android/log.h>
#include <SFML/Graphics.hpp>
#include <android/native_activity.h>
#include <SFML/System/NativeActivity.hpp>
#include "GraphicsLoop.h"

int main(int argc, char *argv[]) {
    assert(ne10_init() == NE10_OK);

    AudioEngine audioEngine;
    audioEngine.startRecording();

    graphics::GraphicsLoop graphicsLoop("ElectroPARTYogram", audioEngine);

    graphicsLoop.loop();
    audioEngine.stopRecording();
}

