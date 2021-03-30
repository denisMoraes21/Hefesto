//
// Created by andre on 1/16/21.
//

#include <cstring>
#include <unistd.h>
#include "cpu.h"

namespace hefesto {

    CPUInfo::CPUInfo() {
        // get total CPUS
        this->total_cpu_count_ = sysconf(_SC_NPROCESSORS_CONF);

        // set to zero at beginning
        this->total_cpu_utilization_ = 0;
        this->total_cpu_time_ = 0;
        this->total_cpu_idle_time_ = 0;
        this->prev_cpu_idle_time_ = 0;
        this->prev_cpu_time_;
    }

    void CPUInfo::refreshGlobal() {

        // prepare scanf pattern
        char cur_pattern[PATTERNSIZE];
        memset(cur_pattern, 0, PATTERNSIZE);
        strncpy(cur_pattern, CPU_GLOBAL_PATTERN, PATTERNSIZE);

        // open /proc/stat
        FILE *stat_file = fopen(SYS_PROC_FILE, "r");
        if(!stat_file)
            return;

        // extract information
        unsigned long user_time = 0;
        unsigned long nice_time = 0;
        unsigned long system_time = 0;
        unsigned long idle_time = 0;
        unsigned long io_wait_time = 0;
        unsigned long irq_time = 0;
        unsigned long soft_irq_time = 0;

        // get total CPU usage
        int flag = fscanf(stat_file, cur_pattern,
                          &user_time, &nice_time,
                          &system_time, &idle_time,
                          &io_wait_time, &irq_time,
                          &soft_irq_time);
        // close file
        fclose(stat_file);

        // validate
        if(flag != 7)
            return;

        // get CPU time
        unsigned long cpu_time = user_time + nice_time + system_time + idle_time +
                                 io_wait_time + irq_time + soft_irq_time;

        // calculate utilization
        unsigned long total_delta_time = cpu_time - this->prev_cpu_time_;
        unsigned long total_idle_time = idle_time - this->prev_cpu_idle_time_;

        if(total_idle_time != 0)
            this->total_cpu_utilization_ = 100 - ((float) total_idle_time * 100 / total_delta_time);
        else
            this->total_cpu_utilization_ = 100;

        // save data
        this->prev_cpu_time_ = cpu_time;
        this->prev_cpu_idle_time_ = idle_time;

        this->total_cpu_time_ = total_delta_time;
        this->total_cpu_idle_time_ = total_idle_time;

    }

    float CPUInfo::getCPUUtilization() {
        return (this->total_cpu_utilization_);
    }

    unsigned long CPUInfo::getCPUTime() {
        return (this->total_cpu_time_);
    }
}
