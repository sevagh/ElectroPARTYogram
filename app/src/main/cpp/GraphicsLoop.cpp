#include "GraphicsLoop.h"

static bool abs_compare_f(float a, float b)
{
    return (std::fabs(a) < std::fabs(b));
}

void graphics::GraphicsLoop::recomputePeriod() {
    period = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::duration<float, std::ratio<1, 1000000>>(tempo/(60.0F*1000000.0F)));
}

void graphics::GraphicsLoop::createBeatTrackArt(const DrawParams *draw) {
    auto viewSize = view.getSize();

    // copy FFT data
    std::vector<float> fftMag(
            audioEngine.recordingCallback.beatDetector.odf.magSpecCopy,
            audioEngine.recordingCallback.beatDetector.odf.magSpecCopy + btrack::OnsetDetectionFunction::FrameSize);

    // copy audio data
    std::vector<float> audioArray(audioEngine.recordingCallback.beatDetector.currentFrame,
            audioEngine.recordingCallback.beatDetector.currentFrame + btrack::BTrack::FrameSize);

    int hopSize = (int)(viewSize.x/(float)btrack::BTrack::FrameSize);
    int pos = 0;

    // draw input waveform from left to right on top
    // draw FFT from left to right on bottom
    auto audioMax = std::max_element(audioArray.begin(), audioArray.end(), abs_compare_f);
    audioOverallMax = std::max(*audioMax, audioOverallMax);
    auto fftMax = std::max_element(fftMag.begin(), fftMag.end(), abs_compare_f);
    fftMagOverallMax = std::max(*fftMax, fftMagOverallMax);
    sf::VertexArray audioCurve(sf::PrimitiveType::LinesStrip, btrack::BTrack::FrameSize);
    sf::VertexArray fftCurve(sf::PrimitiveType::LinesStrip, btrack::BTrack::FrameSize);
    size_t fftIndex = 0;
    for (size_t i = 0; i < btrack::BTrack::FrameSize; ++i) {
        audioCurve[i] = sf::Vertex(sf::Vector2f(pos, (audioArray[i]/audioOverallMax)*0.25F*viewSize.y));
        fftCurve[i] = sf::Vertex(sf::Vector2f(pos, viewSize.y-(fftMag[fftIndex]/fftMagOverallMax)*0.25F*viewSize.y));
        pos += hopSize;
        // fill every other element for FFT since only half is valuable
        if (i % 2) {
            fftIndex++;
        }
    }

    mainWindow.draw(audioCurve);
    mainWindow.draw(fftCurve);

    cumScoreOverallMax = std::max(cumScoreOverallMax, draw->cumScore);
    currentCumScore = draw->cumScore/cumScoreOverallMax; // at most 1.0 for high energy portions
    sf::Color color((uint8_t)(255*currentCumScore), 0, (uint8_t)(255*(1.0-currentCumScore)), (uint8_t)(255*currentCumScore));

    // make a beat circle
    sf::CircleShape circle(viewSize.x/4.0F);

    if (draw->beat) {
        circle.setRadius(circle.getRadius()*1.25F);
    }

    // center the circle
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    circle.setPosition(viewSize.x/2.0F, viewSize.y/2.0F);
    circle.setFillColor(color);
    mainWindow.draw(circle);
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
                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::Escape) {
                        std::exit(0);
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

            mainWindow.display();
            timer--;
        }
    }
}
