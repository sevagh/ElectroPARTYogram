package xyz.sevag.animals_as_meter

import java.util.concurrent.TimeUnit
import android.util.Log

class AudioEngine constructor(private var ui: UiHelper) {
    companion object {
        init {
            System.loadLibrary("animals_as_meter")
        }

        // Native methods
        @JvmStatic external fun create(): Boolean
        @JvmStatic external fun delete()

        @JvmStatic external fun startRecording()
        @JvmStatic external fun stopRecording()

        @JvmStatic external fun GetDrawParams(): Array<Any>
    }

    private var keepRunning = true
    //private var ui: UiHelper

    fun run() {
        while (keepRunning) {
            val drawParams = GetDrawParams()
            val beat = drawParams[0] as Boolean
            val tempo = drawParams[1] as Float
            val cumulativeScore = drawParams[2] as Float
            if (beat) {
                ui.displayBeat(tempo, cumulativeScore)
            }
            TimeUnit.MILLISECONDS.sleep(23)
        }
    }

    fun stopLoop() {
        keepRunning = false
    }

    fun reset() {
        keepRunning = true
    }
}
