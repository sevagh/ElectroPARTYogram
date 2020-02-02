#include "GraphicsLoop.h"

void graphics::GraphicsLoop::createBeatTrackArt(DrawParams &draw) {
    LOGI("BTrack: why not drawing here? %s %f %f", draw.beat ? "true" : "false", draw.tempo, draw.cumScore);
    if (draw.beat) {
        LOGI("BTrack: should be drawing here!");

        auto viewSize = view.getSize();
        sf::CircleShape circle(viewSize.x/4.0F);

        // center the circle
        circle.setOrigin(viewSize.x/2.0F, viewSize.y/2.0F);
        circle.setFillColor(sf::Color::Red);
        circle.setOutlineColor(sf::Color::Blue);

        shapes.push_back(&circle);
    }
}

void graphics::GraphicsLoop::createFingerArt(const float x, const float y) {
    // do nothing
    float z = 3.0F;
}

void graphics::GraphicsLoop::loop() {
    while (mainWindow.isOpen()) {
        while (mainWindow.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    mainWindow.close();
                    break;
                case sf::Event::Resized:
                    view.setSize(event.size.width, event.size.height);
                    view.setCenter(event.size.width / 2.0F, event.size.height / 2.0F);
                    mainWindow.setView(view);
                    break;
                case sf::Event::TouchBegan:
                    if (event.touch.finger == 0) {
                        createFingerArt(event.touch.x, event.touch.y);
                    }
                    break;
                case sf::Event::LostFocus:
                    focus = false;
                    mainWindow.setActive(false);
                    break;
                case sf::Event::GainedFocus:
                    focus = true;
                    mainWindow.setActive(true);
                    break;
                case sf::Event::MouseEntered:
                    mainWindow.create(sf::VideoMode::getDesktopMode(),
                                      "");
                    break;
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::Escape)
                        mainWindow.close();
                    break;
                default:
                    break;
            }

            if (focus) {
                LOGI("GraphicsLoop: focus");
                if (timer >= 30) { // keep a shape for 30 frames
                    // clear the old shapes
                    shapes.clear();
                    timer = 0;
                }
                timer++;

                auto drawParams = audioEngine.GetDrawParams();

                // populate the new shapes vector
                createBeatTrackArt(drawParams);

                mainWindow.clear();
                for (auto shape : shapes) {
                    mainWindow.draw(*shape);
                }

                // if any finger events exist, draw those too
                for (auto shape : fingerShapes) {
                    mainWindow.draw(*shape);
                }

                mainWindow.display();
            }
        }
    }
}
