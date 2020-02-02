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
           , view(mainWindow.getDefaultView())
           , shapes()
           , fingerShapes()
           , timer(0)
       {
           mainWindow.setVerticalSyncEnabled(true);
       };

       void loop();

   private:
       sf::RenderWindow mainWindow;
       AudioEngine audioEngine;
       bool focus;
       sf::View view;
       sf::Event event;
       std::vector<sf::Shape*> shapes;
       std::vector<sf::Shape*> fingerShapes;
       int timer;

       void createBeatTrackArt(DrawParams &draw);
       void createFingerArt(const float x, const float y);
   };
} // namespace graphics

#endif //ELECTROPARTYOGRAM_GRAPHICSLOOP_H
