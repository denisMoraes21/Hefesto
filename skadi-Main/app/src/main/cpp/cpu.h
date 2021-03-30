//
// Created by andre on 1/16/21.
//

#ifndef PROCMON_PROJ_CPU_H
#define PROCMON_PROJ_CPU_H

#include <cstdio>

#define PATTERNSIZE 64

#define SYS_PROC_FILE "/proc/stat"

#define CPU_GLOBAL_PATTERN "cpu  %lu %lu %lu %lu %lu %lu %lu "
#define CPU_SINGLE_PATTERN "cpu%d %%lu %%lu %%lu %%lu %%lu %%lu %%lu"

namespace hefesto
{
    class CPUInfo
    {
    private:

        float total_cpu_utilization_;
        unsigned long total_cpu_time_;
        unsigned long total_cpu_idle_time_;

        unsigned long prev_cpu_time_;
        unsigned long prev_cpu_idle_time_;

        int total_cpu_count_;

        bool fillCPUInfo();
        void fillEmptyCPUInfo();
        void calculateCPUUtil();
        void gatherCPUInfo();

    public:

        CPUInfo();
        void refresh();
        float getCPUUtilization();
        unsigned long getCPUTime();
        void refreshGlobal();

    };
}

#endif //PROCMON_PROJ_CPU_H
