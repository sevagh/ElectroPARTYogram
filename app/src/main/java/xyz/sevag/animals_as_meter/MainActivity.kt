package xyz.sevag.animals_as_meter

import android.Manifest
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.livinglifetechway.quickpermissions.annotations.WithPermissions
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity()  {
    override fun onCreate(savedInstanceState: Bundle?) {
        Log.d(TAG, "onCreate: ")

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        AudioEngine.create()
        verifyRecordPermissions()
        initUI()
    }

    override fun onResume() {
        Log.d(TAG, "onResume: ")
        super.onResume()
    }

    override fun onDestroy() {
        Log.d(TAG, "onDestroy: ")
        super.onDestroy()
        AudioEngine.delete()
    }

    private fun initUI() {
        Log.d(TAG, "initUI: ")

        btnStartRecording.setOnClickListener {
            Log.d(TAG, "btnStartRecording.onClick: ")
            AudioEngine.startRecording()
        }

        btnStopRecording.setOnClickListener {
            Log.d(TAG, "btnStopRecording.onClick: ")
            AudioEngine.stopRecording()
        }

        btnReset.setOnClickListener {
            Log.v(TAG, "btnReset.onClick: Deleting AudioEngine instance")
            AudioEngine.delete()
            Log.v(TAG, "btnReset.onClick: Creating new AudioEngine instance")
            if (AudioEngine.create()) {
                Log.i(TAG, "btnReset.OnClick: New AudioEngine instance has been created")
            } else {
                Log.e(TAG, "btnReset.OnClick: Something went wrong, please check logcat for errors")
            }
        }
    }

    @WithPermissions(permissions = [Manifest.permission.RECORD_AUDIO])
    fun verifyRecordPermissions() {
        Toast.makeText(this, "Permissions granted", Toast.LENGTH_SHORT).show()
    }

    companion object {
        private val TAG = MainActivity::class.java.simpleName
    }
}
