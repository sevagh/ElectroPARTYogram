#include "GraphicsLoop.h"

static bool abs_compare_f(float a, float b)
{
	return (std::fabs(a) < std::fabs(b));
}

void graphics::GraphicsLoop::createBeatTrackArt(const DrawParams* draw)
{
	bool drawBeat = draw->beat;

	if (drawBeat) {
		timer = FramesBeatVisible;
	}

	auto viewSize = view.getSize();

	// copy FFT data
	std::vector<float> fftMag(
	    audioEngine.recordingCallback.beatDetector.odf.magSpecCopy,
	    audioEngine.recordingCallback.beatDetector.odf.magSpecCopy
	        + btrack::OnsetDetectionFunction::FrameSize);

	// copy audio data
	std::vector<float> audioArray(
	    audioEngine.recordingCallback.beatDetector.currentFrame,
	    audioEngine.recordingCallback.beatDetector.currentFrame
	        + btrack::BTrack::FrameSize);

	int hopSize = ( int )(viewSize.x / ( float )btrack::BTrack::FrameSize);
	int pos = 0;

	// draw input waveform from left to right on top
	// draw FFT from left to right on bottom
	auto audioMax = std::max_element(
	    audioArray.begin(), audioArray.end(), abs_compare_f);
	audioOverallMax = std::max(*audioMax, audioOverallMax);
	auto fftMax = std::max_element(fftMag.begin(), fftMag.end(), abs_compare_f);
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
	if (drawBeat) {
		beatAudioArray = audioArray;
		beatFftMag = fftMag;
	}
	std::vector<float>& toUseAudio = audioArray;
	std::vector<float>& toUseFFT = fftMag;
	float heightFactor = 0.25F;

	if ((drawBeat) || (timer > 0)) {
		defaultVertexColor = sf::Color::Green;
		// defaultVertexColor = sf::Color(255, 0, 0,
		// (uint8_t)(255*currentCumScore)); toUseAudio = beatAudioArray;
		// toUseFFT = beatFftMag;
		heightFactor = 0.75;
	}

	sf::PrimitiveType lineType = sf::PrimitiveType::LinesStrip;
	sf::VertexArray audioCurve(lineType, btrack::BTrack::FrameSize);
	sf::VertexArray fftCurve(lineType, btrack::BTrack::FrameSize);

	for (size_t i = 0; i < btrack::BTrack::FrameSize; ++i) {
		audioCurve[i]
		    = sf::Vertex(sf::Vector2f(pos, (toUseAudio[i] / audioOverallMax)
		                                       * heightFactor * viewSize.y),
		                 defaultVertexColor);
		fftCurve[i] = sf::Vertex(
		    sf::Vector2f(pos, viewSize.y
		                          - (toUseFFT[fftIndex] / fftMagOverallMax)
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

	// make a beat circle
	sf::CircleShape circle(viewSize.x / 4.0F);

	if ((drawBeat) || (timer > 0)) {
		// color = sf::Color::Black;
		// color = sf::Color(0, (uint8_t)(255*currentCumScore), 0,
		// (uint8_t)(255*currentCumScore)); // green with variable opacity
		color = sf::Color(0, 255, 0, (uint8_t)(255 * currentCumScore));
		circle.setRadius(circle.getRadius() * 1.05F);
	}

	// center the circle
	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(viewSize.x / 2.0F, viewSize.y / 2.0F);
	circle.setFillColor(color);
	// circle.setOutlineColor(outlineColor);
	// circle.setOutlineThickness(1.0F);

	mainWindow.draw(circle);
	timer--;
}

void graphics::GraphicsLoop::createFingerArt(const float x, const float y)
{
	// do nothing
	LOGI("creating finger art!");
}

void graphics::GraphicsLoop::loop()
{
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
