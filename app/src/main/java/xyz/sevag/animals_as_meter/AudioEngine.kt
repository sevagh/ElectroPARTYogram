package xyz.sevag.animals_as_meter

class AudioEngine {

    companion object {

        init {
            System.loadLibrary("aam")
        }

        // Native methods
        @JvmStatic external fun create(): Boolean
        @JvmStatic external fun delete()

        @JvmStatic external fun startRecording()
        @JvmStatic external fun stopRecording()

        @JvmStatic external fun startPlayingRecordedStream()
        @JvmStatic external fun stopPlayingRecordedStream()
    }
}
