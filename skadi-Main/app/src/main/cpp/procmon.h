#ifndef PROCMON_H
#define PROCMON_H

#define BUFFERSIZE 256
#define HZ 100

#define SYS_PROC_DIR "/proc"
#define SYS_PROC_LOC "/proc/%d"
#define SYS_PROC_STAT "/proc/%d/stat"
#define SYS_PROC_CMD "/proc/%d/cmdline"
#define SYS_PROC_BIN "%d (%255s)"
#define SYS_PROC_PATTERN "%*d %*s %c %d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %lu %*d %*d %*d %*d %d %*lu %lu %lu %lu"
#define SYS_BOOT_TIME "/proc/uptime"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstring>
#include <list>

#include "process.h"
#include "cpu.h"

namespace hefesto {

    class ProcMon
    {
    private:

        std::list<ProcessInfo>      prev_buffer_;    /** previous process buffer*/
        std::list<ProcessInfo>      cur_buffer_;     /** current process buffer*/
        std::vector<ProcessInfo>    process_list_;   /** current process vector */
        CPUInfo                     cpu_info_;       /** cpu info */
        unsigned long               boot_time_;      /** boot time in ms */

        /**
         * gather process information for /proc directory
         * @return true of false
         */
        bool gatherProcesses();

        /**
         * get specific process information by PID
         * @param[in] pid
         * @return success or fail
         */
        bool getProcessInfo(unsigned int pid);

        /**
         * calculate CPU usage of current processes
         * @return success or fail
         */
        void calcCPUUsage();

        /**
         * get process name from /proc/{pid}/cmdline
         * @param[in] pid
         * @param[out] buffer
         * @param[in] size
         * @return success or fail
         */
        bool getProcessName(unsigned int pid, char *buffer, unsigned int size);

        /**
         * get process name from /proc/{pid}/stat
         * @param pid
         * @param buffer
         * @param size
         * @return success or fail
         */
        bool getProcessNameByStat(unsigned int pid, char *buffer, unsigned int size);

        /**
         * prepare to gather processes
         * @return success or fail
         */
        void prepare();

        void finish();

    public:

        /**
         * constructor
         */
        ProcMon();

        /**
         * destructor
         */
        ~ProcMon();

        /**
         * refresh process information
         * @return success or fail
         */
        bool refresh();

        void getBootTime();

        std::vector<ProcessInfo> getData();

    };
}

#endif /* PROCESS_H */