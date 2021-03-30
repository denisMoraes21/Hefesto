package com.hefesto.skadi

import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Color
import android.os.BatteryManager
import android.os.PowerManager

class Thermometer {

    companion object {

        fun getBatteryTemperature(context: Context): String {


            val intent_filter = IntentFilter(Intent.ACTION_BATTERY_CHANGED)
            val battery_status = context.registerReceiver(null, intent_filter)
            val temp_int: Int =
                (battery_status?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0) ?: 0) / 10
            return temp_int.toString()
        }

        fun getThermalStatus(context: Context) : String {
            var PM = context.getSystemService(Context.POWER_SERVICE) as PowerManager
            return if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q) {
                when(PM.currentThermalStatus){
                    0 -> "Neutro"
                    1 -> "Luz"
                    2 -> "Moderado"
                    3 -> "Grave"
                    4 -> "Crítico"
                    5 -> "Emergência"
                    6 -> "Desligamento"
                    else -> throw IllegalArgumentException("Erro de leitura da temperatura: $this")
                }
            } else {
                "Moderado"
            }
        }

        fun selectColorStatus(context: Context) : Int {
            var PM = context.getSystemService(Context.POWER_SERVICE) as PowerManager
            return if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q) {
                when (PM.currentThermalStatus) {
                    0 -> Color.GREEN
                    1 -> Color.GREEN
                    2 -> Color.GREEN
                    3 -> Color.YELLOW
                    4 -> Color.YELLOW
                    5 -> Color.RED
                    6 -> Color.RED
                    else -> throw IllegalArgumentException("Erro de leitura da temperatura: $this")
                }
            } else {
                Color.GREEN
            }
        }

    }

}