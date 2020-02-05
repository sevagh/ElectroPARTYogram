#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "AudioEngine.h"
#include <optional>
#include <chrono>

#ifndef ELECTROPARTYOGRAM_GRAPHICSLOOP_H
#define ELECTROPARTYOGRAM_GRAPHICSLOOP_H

namespace graphics {
   class GraphicsLoop {
   public:
       GraphicsLoop(const char *appName,
               AudioEngine &engine)
           : mainWindow(sf::RenderWindow(sf::VideoMode::getDesktopMode(),
                   appName))
           , audioEngine(engine)
           , focus(true)
           , touch(true)
           , timer(0)
           , view(mainWindow.getDefaultView())
           , fftMagOverallMax(-FLT_MAX)
           , audioOverallMax(-FLT_MAX)
           , cumScoreOverallMax(-FLT_MAX)
           , currentCumScore(0.0F)
           , tempo(100) // start at 100bpm
           , period(std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::duration<float, std::ratio<1, 1000000>>(tempo/(60.0F*1000000.0F))))
           , drawOuterCircle(false)
       {
           mainWindow.setVerticalSyncEnabled(true);
       };

       void loop();

   private:
       static constexpr int FramesVisible = 128;
       sf::RenderWindow mainWindow;
       AudioEngine audioEngine;
       bool focus;
       bool touch;
       float touch_pos_1;
       float touch_pos_2;
       float fftMagOverallMax;
       float audioOverallMax;
       float cumScoreOverallMax;
       float currentCumScore;
       float tempo;
       std::chrono::microseconds period;
       std::chrono::steady_clock::time_point lastBeat;
       int timer;
       sf::View view;
       sf::Event event;
       bool drawOuterCircle;

       void createBeatTrackArt(const DrawParams *draw);
       void createFingerArt(const float x, const float y);
       void recomputePeriod();
   };
} // namespace graphics

#endif //ELECTROPARTYOGRAM_GRAPHICSLOOP_H
