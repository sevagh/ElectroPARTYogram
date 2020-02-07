#include "AudioEngine.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>

#ifndef ELECTROPARTYOGRAM_GRAPHICSLOOP_H
#define ELECTROPARTYOGRAM_GRAPHICSLOOP_H

namespace graphics {
class GraphicsLoop {
public:
	GraphicsLoop(const char* appName, AudioEngine& engine)
	    : mainWindow(sf::RenderWindow(sf::VideoMode::getDesktopMode(), appName))
	    , audioEngine(engine)
	    , focus(true)
	    , timer(0)
	    , view(mainWindow.getDefaultView())
	    , fftMagOverallMax(-FLT_MAX)
	    , audioOverallMax(-FLT_MAX)
	    , cumScoreOverallMax(-FLT_MAX)
	    , currentCumScore(0.0F)
	{
		mainWindow.setVerticalSyncEnabled(true);
	};

	void loop();

private:
	static constexpr int FramesBeatVisible = 30;
	sf::RenderWindow mainWindow;
	AudioEngine audioEngine;
	bool focus;
	float fftMagOverallMax;
	float audioOverallMax;
	float cumScoreOverallMax;
	float currentCumScore;
	float* fftMagArray;
	float* audioArray;
	int timer;
	sf::View view;
	sf::Event event;

	void createBeatTrackArt(const DrawParams* draw);
};
} // namespace graphics

#endif // ELECTROPARTYOGRAM_GRAPHICSLOOP_H
