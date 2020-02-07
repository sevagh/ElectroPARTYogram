#include "GraphicsLoop.h"

static bool abs_compare_f(float a, float b)
{
	return (std::fabs(a) < std::fabs(b));
}

void graphics::GraphicsLoop::createBeatTrackArt(const DrawParams* draw)
{
	if (draw->beat) {
		timer = FramesBeatVisible;
	}

	auto viewSize = view.getSize();

	fftMagArray = audioEngine.recordingCallback.beatDetector.odf.magSpecCopy;
	audioArray = audioEngine.recordingCallback.beatDetector.currentFrame;

	int hopSize = ( int )(viewSize.x / ( float )btrack::BTrack::FrameSize);
	int pos = 0;

	// draw input waveform from left to right on top
	// draw FFT from left to right on bottom
	auto audioMax = std::max_element(
	    audioArray, audioArray + btrack::BTrack::FrameSize, abs_compare_f);
	audioOverallMax = std::max(*audioMax, audioOverallMax);
	auto fftMax = std::max_element(
	    fftMagArray, fftMagArray + btrack::BTrack::FrameSize, abs_compare_f);
	fftMagOverallMax = std::max(*fftMax, fftMagOverallMax);

	cumScoreOverallMax = std::max(cumScoreOverallMax, draw->lastOnset);
	currentCumScore
	    = draw->lastOnset
	      / cumScoreOverallMax; // at most 1.0 for high energy portions
	sf::Color color((uint8_t)(255 * currentCumScore), 0,
	                (uint8_t)(255 * (1.0 - currentCumScore)),
	                (uint8_t)(255 * currentCumScore));
	sf::Color outlineColor = sf::Color::Black;

	size_t fftIndex = 0;
	sf::Color defaultVertexColor = sf::Color::White;

	float heightFactor = 0.25F;

	if ((draw->beat) || (timer > 0)) {
		defaultVertexColor = sf::Color::Green;
		heightFactor = 0.75;
	}

	sf::VertexArray audioCurve(
	    sf::PrimitiveType::LinesStrip, btrack::BTrack::FrameSize);
	sf::VertexArray fftCurve(
	    sf::PrimitiveType::LinesStrip, btrack::BTrack::FrameSize);

	for (size_t i = 0; i < btrack::BTrack::FrameSize; ++i) {
		audioCurve[i]
		    = sf::Vertex(sf::Vector2f(pos, (audioArray[i] / audioOverallMax)
		                                       * heightFactor * viewSize.y),
		                 defaultVertexColor);
		fftCurve[i] = sf::Vertex(
		    sf::Vector2f(pos, viewSize.y
		                          - (fftMagArray[fftIndex] / fftMagOverallMax)
		                                * heightFactor * viewSize.y),
		    defaultVertexColor);
		pos += hopSize;
		// fill every other element for FFT since only half is valuable
		if (i % 2) {
			fftIndex++;
		}
	}

	mainWindow.draw(audioCurve);
	mainWindow.draw(fftCurve);

	sf::CircleShape circle(viewSize.x / 4.0F);

	if ((draw->beat) || (timer > 0)) {
		color = sf::Color(0, 255, 0, (uint8_t)(255 * currentCumScore));
		circle.setRadius(circle.getRadius() * 1.05F);
	}

	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(viewSize.x / 2.0F, viewSize.y / 2.0F);
	circle.setFillColor(color);

	mainWindow.draw(circle);
	timer--;
}

void graphics::GraphicsLoop::loop()
{
	while (mainWindow.isOpen()) {
		while (mainWindow.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				mainWindow.close();
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

			createBeatTrackArt(drawParams);

			mainWindow.display();
			timer--;
		}
	}
}
