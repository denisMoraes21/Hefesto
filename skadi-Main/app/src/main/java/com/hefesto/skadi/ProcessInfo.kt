package com.hefesto.skadi

class ProcessInfo {

    var name : String = ""
    var owner : String = ""

    var uid : Int = 0
    var pid : Int = 0
    var rss : Int = 0
    var vsz : Int = 0
    var ppid : Int = 0
    var process_status : Int = 0
    var priority_level : Int = 0
    var used_user_time : Int = 0
    var used_system_time : Int = 0
    var start_time : Int = 0
    var cpu_time : Int = 0
    var thread_count : Int = 0

    var cpu_usage : Float = 0.0f

}