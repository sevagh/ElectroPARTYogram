#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "AudioEngine.h"
#include <optional>

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
       {
           mainWindow.setVerticalSyncEnabled(true);
       };

       void loop();

   private:
       static constexpr int FramesVisible = 32;
       sf::RenderWindow mainWindow;
       AudioEngine audioEngine;
       bool focus;
       bool touch;
       float touch_pos_1;
       float touch_pos_2;
       int timer;
       sf::View view;
       sf::Event event;

       void createBeatTrackArt(const DrawParams *draw);
       void createFingerArt(const float x, const float y);
   };
} // namespace graphics

#endif //ELECTROPARTYOGRAM_GRAPHICSLOOP_H
