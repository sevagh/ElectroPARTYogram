package xyz.sevag.animals_as_meter

import android.Manifest
import android.app.NativeActivity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.livinglifetechway.quickpermissions.annotations.WithPermissions


class MainActivity : AppCompatActivity() {
    companion object {
        init {
            System.loadLibrary("animals_as_meter")
        }

        private val TAG = MainActivity::class.java.simpleName
    }

    var streamStarted = false

    override fun onCreate(savedInstanceState: Bundle?) {
        Log.d(TAG, "onCreate: ")

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        verifyRecordPermissions()
        AudioEngine.create()
        val intent = Intent(this, NativeActivity::class.java)
        startActivity(intent)
    }

    override fun onResume() {
        Log.d(TAG, "onResume: ")
        startStream()
        super.onResume()
    }

    fun startStream() {
        if (!streamStarted) {
            AudioEngine.startRecording()
            streamStarted = true
        }
    }

    fun stopStream() {
        if (streamStarted) {
            AudioEngine.stopRecording()
            streamStarted = false
        }
    }

    override fun onPause() {
        Log.d(TAG, "onPause: ")
        stopStream()
        super.onPause()
    }

    override fun onStop() {
        Log.d(TAG, "onStop: ")
        stopStream()
        super.onStop()
    }

    override fun onDestroy() {
        Log.d(TAG, "onDestroy: ")
        super.onDestroy()
        AudioEngine.delete()
    }

    @WithPermissions(permissions = [Manifest.permission.RECORD_AUDIO])
    fun verifyRecordPermissions() {
        Toast.makeText(this, "Permissions granted", Toast.LENGTH_SHORT).show()
    }
}

