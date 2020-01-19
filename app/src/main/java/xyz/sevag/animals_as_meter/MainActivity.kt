package xyz.sevag.animals_as_meter

import android.Manifest
import android.app.NativeActivity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_main.aamStartButton
import kotlinx.android.synthetic.main.activity_main.aamInfoButton
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

        verifyRecordPermissions()

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        aamInfoButton.setOnClickListener {
            Toast.makeText(this, "Created by Sevag Hanssian, 2020\nhttps://github.com/sevagh/Animals-as-Meter", Toast.LENGTH_LONG).show()
        }

        aamStartButton.setOnClickListener {
            val intent = Intent(this, NativeActivity::class.java)
            startActivity(intent)
        }
    }

    @WithPermissions(permissions = [Manifest.permission.RECORD_AUDIO])
    fun verifyRecordPermissions() {
        Toast.makeText(this, "Permissions granted", Toast.LENGTH_SHORT).show()
    }
}

