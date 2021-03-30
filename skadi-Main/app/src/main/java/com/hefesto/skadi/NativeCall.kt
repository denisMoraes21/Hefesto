package com.hefesto.skadi

class NativeCall {

    companion object {

        init
        {
            System.loadLibrary("native")
        }

        external fun getProcessInfo() : List<ProcessInfo>
        external fun refreshProcessList() : Unit
        external fun getTemperature() : String
    }


}