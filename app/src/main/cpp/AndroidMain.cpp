#include "AudioEngine.h"
#include "GraphicsLoop.h"
#include "NE10.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/NativeActivity.hpp>
#include <android/log.h>
#include <android/native_activity.h>

int main(int argc, char* argv[])
{
	assert(ne10_init() == NE10_OK);

	AudioEngine audioEngine;
	audioEngine.startRecording();

	graphics::GraphicsLoop graphicsLoop("ElectroPARTYogram", audioEngine);

	graphicsLoop.loop();
	audioEngine.stopRecording();
}
