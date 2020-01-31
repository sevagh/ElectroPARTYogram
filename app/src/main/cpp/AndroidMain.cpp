#include "AudioEngine.h"
#include "NE10.h"
#include <android/log.h>
#include <android_native_app_glue.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <android/native_activity.h>
#include <SFML/System/NativeActivity.hpp>

// Process the next main command.
void handle_cmd(android_app* app, int32_t cmd)
{
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            break;
        default:
            __android_log_print(ANDROID_LOG_INFO, "Animals-as-Meter",
                                "event not handled: %d", cmd);
    }
}

const float pi = 3.14159265F;
unsigned int sampleRate = 48000;
unsigned int bufferFrames = 512; // 512 sample frames
const int bandNumber = 128;
const int width = bufferFrames / bandNumber;
const int historyValues = sampleRate / (bufferFrames * 2);

const float nodeRadius = 100;
const float angularWidth = 2.0 * pi / bandNumber;
const float barWidth = angularWidth * nodeRadius;

#include <android/native_activity.h>
#include <SFML/System/NativeActivity.hpp>

int main(int argc, char *argv[]) {
}

//void android_main(struct android_app* app)
//{
int main(int argc, char *argv[])
    ANativeActivity *activity1 = app->activity;
    int32_t sdkVer1 = activity1->sdkVersion;
    ANativeActivity *activity = sf::getNativeActivity();
    int32_t sdkVer2 = activity1->sdkVersion;

    app->onAppCmd = handle_cmd;

    // Used to poll the events in the main loop
    int events;
    android_poll_source* source;

    assert(ne10_init() == NE10_OK);

    AudioEngine audioEngine;
    audioEngine.startRecording();

    std::vector<sf::RectangleShape> bars(bandNumber), historyBars(bandNumber);
    int i;
    for (i = 0; i < bandNumber; i++) {
        bars[i].setFillColor(sf::Color(200, (256 / bandNumber) * i, (256 / bandNumber) * i));
        historyBars[i].setFillColor(sf::Color(20, 40, 250, 100));
    }

    sf::CircleShape node(nodeRadius, 48);
    node.setFillColor(sf::Color::Black);
    node.setOrigin(nodeRadius, nodeRadius);

    sf::RectangleShape testShape;
    testShape.setFillColor(sf::Color::Yellow);


    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Animals-as-Meter");
    window.setVerticalSyncEnabled(true);
    int frameCounter = 0;

    // Main loop
    while ((app->destroyRequested == 0) && window.isOpen()) {
        sf::View view = window.getDefaultView();

        bool focus = true;

        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    view.setSize(event.size.width, event.size.height);
                    view.setCenter(event.size.width / 2, event.size.height / 2);
                    window.setView(view);
                    break;
                    //case sf::Event::TouchBegan:
                    //    if (event.touch.finger == 0)
                    //    {
                    //        image.setPosition(event.touch.x, event.touch.y);
                    //    }
                    //    break;
                case sf::Event::LostFocus:
                    focus = false;                    //don't draw, if the window is not shown
                    LOGI("LOST FOCUS!");
                    break;
                case sf::Event::GainedFocus:
                    focus = true;                    //draw if the window is shown
                    LOGI("GAINED FOCUS!");
                    break;
                case sf::Event::MouseEntered: // mouse entered event is called on activity resume
                    LOGI("MOUSE ENTERED!");
                    window.create(sf::VideoMode::getDesktopMode(),
                                  "");   //recreating the window circumvents a nasty sfml-bug where on activityResume the egl_surface is not valid anymore
                    break;
            }
        }

        if (focus) {
            auto drawParams = audioEngine.GetDrawParams();
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            //gen shapes
            node.setPosition(640, 450);
            testShape.setSize(sf::Vector2f((int) barWidth, 200));
            for (i = 0; i < bandNumber; i++) {
                double height =
                        log10(audioEngine.recordingCallback.beatDetector.onsetDF.buffer[i]) * 50;
                double historyHeight =
                        log10(audioEngine.recordingCallback.beatDetector.cumulativeScore.buffer[i]) *
                        50;
                bars[i].setSize(sf::Vector2f(barWidth, height - 50));
                bars[i].setOrigin(bars[i].getSize().x / 2, bars[i].getSize().y);
                bars[i].setRotation(angularWidth * i * 180.0 / pi);
                bars[i].setPosition(node.getPosition().x + nodeRadius * sin(angularWidth * i),
                                    node.getPosition().y - nodeRadius * cos(angularWidth * i));

                historyBars[i].setSize(sf::Vector2f(barWidth, historyHeight - 50));
                //            historyBars[i].setPosition(i*(width*2 + 1), 1000 - historyHeight);

                historyBars[i].setOrigin(historyBars[i].getSize().x / 2,
                                         historyBars[i].getSize().y);
                historyBars[i].setRotation(angularWidth * i * 180.0 / pi);
                historyBars[i].setPosition(
                        node.getPosition().x + nodeRadius * sin(angularWidth * i),
                        node.getPosition().y - nodeRadius * cos(angularWidth * i));
                if (height > historyHeight * 1.07 || height < 1) {
                    bars[i].setFillColor(sf::Color::Green);
                    node.setFillColor(sf::Color::Green);
                    frameCounter = 0;
                } else {
                    if (frameCounter > 5) {
                        bars[i].setFillColor(
                                sf::Color(200, (256 / bandNumber) * i, (256 / bandNumber) * i));
                        node.setFillColor(sf::Color::Black);
                    }
                }
            }
            frameCounter++;
            //        testShape.setSize(sf::Vector2f(400, 400));
            testShape.setOrigin(testShape.getSize().x / 2, testShape.getSize().y);
            testShape.setRotation(30);
            testShape.setPosition(node.getPosition().x + nodeRadius * sin(pi / 6),
                                  node.getPosition().y - nodeRadius * cos(pi / 6));
            window.clear();
            //draw
            //        window.draw(testShape);
            window.draw(node);
            for (i = 0; i < bandNumber; i++) {
                window.draw(bars[i]);
                window.draw(historyBars[i]);
            }
            window.display();
        }
    }

    audioEngine.stopRecording();
}

