package xyz.sevag.animals_as_meter

import android.Manifest
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.livinglifetechway.quickpermissions.annotations.WithPermissions
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity()  {
    var streamStarted = false

    override fun onCreate(savedInstanceState: Bundle?) {
        Log.d(TAG, "onCreate: ")

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        verifyRecordPermissions()
        AudioEngine.create()
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

    companion object {
        private val TAG = MainActivity::class.java.simpleName
    }
}
