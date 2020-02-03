#include "GraphicsLoop.h"

void graphics::GraphicsLoop::createBeatTrackArt(const DrawParams *draw) {
    if (draw->beat) {
        auto viewSize = view.getSize();
        sf::CircleShape circle(viewSize.x/4.0F);

        // center the circle
        circle.setOrigin(viewSize.x/2.0F, viewSize.y/2.0F);
        circle.setFillColor(sf::Color::Red);
        circle.setOutlineColor(sf::Color::Blue);

        mainWindow.draw(circle);
    }
}

void graphics::GraphicsLoop::createFingerArt(const float x, const float y) {
    // do nothing
    LOGI("creating finger art!");
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
                        touch = true;
                        touch_pos_1 = event.touch.x;
                        touch_pos_2 = event.touch.y;
                    }
                    break;
                case sf::Event::LostFocus:
                    focus = false;
                    break;
                case sf::Event::GainedFocus:
                    focus = true;
                    break;
                case sf::Event::MouseEntered:
                    mainWindow.create(sf::VideoMode::getDesktopMode(),
                                      "");
                    break;
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::Escape) {
                        mainWindow.setVisible(false);
                        //mainWindow.close();
                    }
                    break;
                default:
                    break;
            }
        }

        if (focus) {
            auto drawParams = audioEngine.GetDrawParams();

            mainWindow.clear();

            if (touch) {
                touch = false; // clear it for the next go around the loop
                createFingerArt(touch_pos_1, touch_pos_2);
            }

            createBeatTrackArt(drawParams);
            // populate the new shapes vector
            //if (!drawParams.beat) {
            //    shapes.clear();
            //    createBeatTrackArt(drawParams);
            //}

            mainWindow.display();
        }
    }
}
