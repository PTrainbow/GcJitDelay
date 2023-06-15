package com.ptrain.art

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import com.ptrain.art.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        Thread {
            while (true){
                Thread.sleep(1000)
                val arr = LongArray(4096*1024){1}
                Log.e("art-hook", "new Long Array, size = ${arr.size}")
            }
        }.start()

    }

    companion object {
        // Used to load the 'art' library on application startup.
        init {
            System.loadLibrary("art")
        }
    }
}