package com.hefesto.skadi

import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.ContextMenu
import android.view.MenuItem
import android.view.View
import android.widget.*
import android.widget.AdapterView.AdapterContextMenuInfo
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.core.view.get
import com.hefesto.skadi.R
import java.util.jar.Manifest

class ProcessActivity : AppCompatActivity() {

    lateinit var adp_pi: MyAdapter
    lateinit var lv_proc: ListView
    lateinit var proc_list : List<ProcessInfo>

    lateinit var tv_battery_temp : TextView
    lateinit var tv_cpu_temp : TextView

    lateinit var battery_temp : String
    lateinit var cpu_temp : String

    var sort_selector : Int = 0

    private val handler = Handler(Looper.getMainLooper())

    private val refresh_process = object : Runnable {
        override fun run() {
            loadData()
            draw_task()
            handler.postDelayed(this, 5000)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.process_activity)

        initView()
        handler.post(refresh_process)

    }

    override fun onStart() {
        super.onStart()
    }

    protected override fun onPause() {
        super.onPause()
        handler.removeCallbacks(refresh_process)
        finish()
    }

    override fun onCreateContextMenu(
        menu: ContextMenu?,
        v: View?,
        menuInfo: ContextMenu.ContextMenuInfo?
    ) {
        super.onCreateContextMenu(menu, v, menuInfo)
        getMenuInflater().inflate(R.menu.process_menu, menu)
    }

    override fun onContextItemSelected(item: MenuItem): Boolean {
        val info = item.menuInfo as AdapterContextMenuInfo
        var proc = lv_proc.getItemAtPosition(info.position) as ProcessInfo
        when(item.itemId) {
            R.id.kill -> {
                if (proc != null) {
                    if (ContextCompat.checkSelfPermission(this, android.Manifest.permission.KILL_BACKGROUND_PROCESSES) == PackageManager.PERMISSION_GRANTED) {
                        android.os.Process.killProcess(proc.pid)
                        Toast.makeText(this, "Selected process killed.", Toast.LENGTH_LONG).show()
                    }
                    else {
                        Toast.makeText(this, "No root permissions.", Toast.LENGTH_LONG).show()
                    }
                }
                else
                    Toast.makeText(this, "No process found.", Toast.LENGTH_LONG).show()
                return true
            }
            else -> {}
        }
        return super.onContextItemSelected(item)
    }

    // Vincula as variaveis aos IDs dos elementos visuais
    private fun initView()
    {
        tv_cpu_temp = findViewById(R.id.procmon_tv_cpu_temp)
        tv_battery_temp = findViewById(R.id.procmon_tv_battery_temp)
        lv_proc = findViewById(R.id.list)
        registerForContextMenu(lv_proc)
    }

    // Atualiza a lista de processos
    private fun loadData()
    {
        NativeCall.refreshProcessList()
        proc_list = NativeCall.getProcessInfo()
        proc_list = proc_list.filter { it.owner.subSequence(0, 2) == "u0" }     // filtra apenas os processos de usuarios
        proc_list = when(sort_selector) {
            0 -> proc_list.sortedByDescending { it.cpu_usage }
            1 -> proc_list.sortedByDescending { it.name }
            2 -> proc_list.sortedByDescending { it.pid }
            3 -> proc_list.sortedBy { it.owner }
            else -> proc_list.sortedByDescending { it.cpu_usage }
        }
        if(Build.VERSION.SDK_INT <= 23)
            cpu_temp = NativeCall.getTemperature()
        battery_temp = Thermometer.getBatteryTemperature(this)
    }

    // Atualiza a ListViewer
    private fun draw_task(){
        tv_battery_temp.setText(battery_temp.subSequence(0, 2).padEnd(3,'º').padEnd(4,'C'))
        if(Build.VERSION.SDK_INT <= 23)
            tv_cpu_temp.setText(cpu_temp.subSequence(0, 2).padEnd(3,'º').padEnd(4,'C'))
        adp_pi = MyAdapter(this@ProcessActivity, R.layout.single_item, proc_list)
        lv_proc.setAdapter(adp_pi)
        lv_proc.invalidateViews()
    }

}