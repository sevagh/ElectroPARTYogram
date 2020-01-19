#include "SoundRecording.h"

int32_t SoundRecording::write(const int16_t* sourceData, int32_t numSamples)
{

	LOGD(TAG, "write(): ");

	// Check that data will fit, if it doesn't then create new array,
	// copy over old data to new array then delete old array and
	// point mData to new array.
	if (mWriteIndex + numSamples > (kMaxSamples * mIteration)) {

		LOGW(TAG, "write(): mWriteIndex + numSamples > kMaxSamples");
		mIteration++;

		int32_t newSize = kMaxSamples * mIteration;

		auto* newData = new int16_t[newSize]{0};
		std::copy(mData, mData + mTotalSamples, newData);

		delete[] mData;
		mData = newData;
	}

	for (int i = 0; i < numSamples; ++i) {
		mData[mWriteIndex++] = sourceData[i] * gain_factor;
	}

	mTotalSamples += numSamples;

	return numSamples;
}

int32_t SoundRecording::read(int16_t* targetData, int32_t numSamples)
{

	LOGD(TAG, "read(): ");

	int32_t framesRead = 0;

	while (framesRead < numSamples && mReadIndex < mTotalSamples) {
		targetData[framesRead++] = mData[mReadIndex++];
		if (mIsLooping && mReadIndex == mTotalSamples)
			mReadIndex = 0;
	}

	return framesRead;
}
