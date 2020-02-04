#include "GraphicsLoop.h"

void graphics::GraphicsLoop::createBeatTrackArt(const DrawParams *draw) {
    // copy FFT data
    std::array<ne10_fft_cpx_float32_t, btrack::OnsetDetectionFunction::FrameSize> fftArray(audioEngine.recordingCallback.beatDetector.odf.complexOut);

    // copy audio data
    std::vector<float> audioArray(audioEngine.recordingCallback.beatDetector.currentFrame,
            audioEngine.recordingCallback.beatDetector.currentFrame + btrack::BTrack::FrameSize);

    LOGI("size of audio array: %ld", audioArray.size());
    LOGI("size of FFT array: %ld", fftArray.size());

    if (audioArray.size() == btrack::BTrack::FrameSize) {
        // amplitude of audio in
        sf::VertexArray curve(sf::PrimitiveType::LinesStrip, btrack::BTrack::FrameSize);
        for (size_t i = 0; i < btrack::BTrack::FrameSize; ++i) {
            curve[i] = sf::Vertex(sf::Vector2f(i, audioArray[i]));
        }
        mainWindow.draw(curve);
    }

    // magnitude of FFT
    sf::VertexArray curve(sf::PrimitiveType::Points, btrack::BTrack::FrameSize);
    for (size_t i = 0; i < btrack::BTrack::FrameSize; ++i) {
        curve[i] = sf::Vertex(sf::Vector2f(i, sqrtf(fftArray[i].r * fftArray[i].i)));
    }
    mainWindow.draw(curve);

    if (draw->beat) {
        auto viewSize = view.getSize();
        sf::CircleShape circle(viewSize.x/4.0F);

        LOGI("drawing something on the beat!");
        // center the circle
        circle.setOrigin(viewSize.x/2.0F, viewSize.y/2.0F);
        circle.setFillColor(sf::Color::Red);
        circle.setOutlineColor(sf::Color::Blue);

        mainWindow.draw(circle);
        timer = FramesVisible;
    }
}

void graphics::GraphicsLoop::createFingerArt(const float x, const float y) {
    // do nothing
    LOGI("creating finger art!");
}

void graphics::GraphicsLoop::loop() {
    while (mainWindow.isOpen()) {
        sf::View view = mainWindow.getDefaultView();

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

            //if (timer == 0)
            mainWindow.clear(sf::Color::Red);

            if (touch) {
                touch = false; // clear it for the next go around the loop
                createFingerArt(touch_pos_1, touch_pos_2);
            }

            createBeatTrackArt(drawParams);

            mainWindow.display();
            timer--;
        }
    }
}
