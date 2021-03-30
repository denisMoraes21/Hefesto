package com.hefesto.skadi

import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import androidx.appcompat.app.AppCompatActivity
import com.hefesto.skadi.R

class SplashActivity : AppCompatActivity() {

    val handler : Handler = Handler(Looper.getMainLooper())

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.splash_screen)

        handler.postDelayed(do_nothing, 3000)

    }

    private val do_nothing = object : Runnable
    {
        override fun run() {
            val intent : Intent = Intent(this@SplashActivity, MainActivity::class.java)
            this@SplashActivity.startActivity(intent)
            finish()
        }
    }

}