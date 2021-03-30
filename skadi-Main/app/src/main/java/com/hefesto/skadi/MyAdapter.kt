package com.hefesto.skadi

import android.app.Activity
import android.content.Context
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.ImageView
import android.widget.TextView
import android.content.pm.PackageManager
import android.content.pm.ApplicationInfo;
import com.hefesto.skadi.R

class MyAdapter(
    private val mcontext: Context,
    private val layout_resource_id: Int,
    private val data: List<ProcessInfo>
) : ArrayAdapter<ProcessInfo?>(
    mcontext, layout_resource_id, data
) {
    override fun getView(position: Int, convert_view: View?, parent: ViewGroup): View {
        var row = convert_view
        var holder: ViewHolder? = null
        var ai: ApplicationInfo
        if (row == null) {
            val inflater = (context as Activity).layoutInflater
            row = inflater.inflate(layout_resource_id, parent, false)
            holder = ViewHolder()
            holder.textView1 = row.findViewById<View>(R.id.pid_label) as TextView
            holder.textView2 = row.findViewById<View>(R.id.name_label) as TextView
            holder.textView3 = row.findViewById<View>(R.id.load_label) as TextView
            holder.imageView1 = row.findViewById<View>(R.id.icon1) as ImageView
            row.tag = holder
        } else {
            holder = row.tag as ViewHolder
        }
        val proc = data[position]

        holder.textView1!!.setText(proc.pid.toString())
        holder.textView3!!.setText(proc.cpu_usage.toInt().toString())

        try {
            ai = mcontext.getPackageManager().getApplicationInfo(proc.name, PackageManager.GET_META_DATA)
            holder.textView2!!.setText(mcontext.getPackageManager().getApplicationLabel(mcontext.getPackageManager().getApplicationInfo(proc.name, PackageManager.GET_META_DATA)).toString())
            holder.imageView1!!.setImageDrawable(
                ai.loadIcon(mcontext.getPackageManager())
            )
        } catch (e : PackageManager.NameNotFoundException) {
            holder.textView2!!.setText(proc.name)
            holder.imageView1!!.setImageDrawable(mcontext.getPackageManager().defaultActivityIcon)
        }
        return row!!
    }

    internal class ViewHolder {
        var textView1: TextView? = null
        var textView2: TextView? = null
        var textView3: TextView? = null
        var imageView1: ImageView? = null
    }
}