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

    override fun onCreate(savedInstanceState: Bundle?) {
        Log.d(TAG, "onCreate: ")

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        verifyRecordPermissions()
        val intent = Intent(this, NativeActivity::class.java)
        startActivity(intent)
    }

    @WithPermissions(permissions = [Manifest.permission.RECORD_AUDIO])
    fun verifyRecordPermissions() {
        Toast.makeText(this, "Permissions granted", Toast.LENGTH_SHORT).show()
    }
}

