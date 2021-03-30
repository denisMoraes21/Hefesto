package com.hefesto.skadi

import android.content.Intent
import android.graphics.Color
import android.hardware.SensorManager
import android.os.*
import androidx.appcompat.app.AppCompatActivity
import android.widget.ImageButton
import android.widget.TextView
import com.hefesto.skadi.R

class MainActivity : AppCompatActivity() {

    lateinit var cpu_temp : String
    lateinit var battery_temp : String
    lateinit var thermal_status: String

    lateinit var ib_process : ImageButton
    lateinit var tv_cpu_temp : TextView
    lateinit var tv_battery_temp : TextView
    lateinit var tv_thermal_status : TextView

    private val handler = Handler(Looper.getMainLooper())

    override fun onCreate(savedInstanceState: Bundle?)
    {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        ib_process          = this.findViewById(R.id.button_intent_procmon)
        tv_cpu_temp         = this.findViewById(R.id.label_cpu_temp)
        tv_battery_temp     = this.findViewById(R.id.label_battery_temp)
        tv_thermal_status   = this.findViewById(R.id.label_status)

        ib_process?.setOnClickListener {
            val intent = Intent(this, ProcessActivity::class.java)
            this.startActivity(intent)
        }

        handler.post(refresh_temperature)

    }

    private val refresh_temperature = object : Runnable
    {
        override fun run()
        {
            if(Build.VERSION.SDK_INT <= 23)
                cpu_temp = NativeCall.getTemperature()
            battery_temp = Thermometer.getBatteryTemperature(this@MainActivity)
            thermal_status = Thermometer.getThermalStatus(this@MainActivity)
            draw_task()
            handler.postDelayed(this, 5000)
        }
    }

    private fun draw_task()
    {
        if(Build.VERSION.SDK_INT <= 23)
            tv_cpu_temp.setText(cpu_temp.subSequence(0, 2).padEnd(3,'º').padEnd(4,'C'))
        tv_battery_temp.setText(battery_temp.subSequence(0, 2).padEnd(3,'º').padEnd(4,'C'))
        tv_thermal_status.setText(thermal_status)
        tv_thermal_status.setTextColor(Thermometer.selectColorStatus(this))
    }

    override fun onPause() {
        super.onPause()
        handler.removeCallbacks(refresh_temperature)
    }

    override fun onResume() {
        super.onResume()
        handler.post(refresh_temperature)
    }

    override fun onDestroy() {
        super.onDestroy()
        handler.removeCallbacks(refresh_temperature)
        this.finish()
    }

}
