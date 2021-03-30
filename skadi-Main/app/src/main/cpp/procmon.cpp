#include <iostream>
#include <vector>
#include "procmon.h"

namespace hefesto {

    ProcMon::ProcMon() {
        this->boot_time_ = 0;
        this->prev_buffer_.empty();
        this->cur_buffer_.empty();
    }

    ProcMon::~ProcMon() {
        this->prev_buffer_.empty();
        this->cur_buffer_.empty();
    }

    bool ProcMon::gatherProcesses() {
        // refresh CPU usage
        this->cpu_info_.refreshGlobal();

        // open /proc directory
        DIR *proc_dir = opendir(SYS_PROC_DIR);
        if (proc_dir == NULL)
            return false;

        // read each process directory
        struct dirent *pd;
        while ((pd = readdir(proc_dir)) != NULL) {
            if (isdigit(pd->d_name[0]))
                if (atoi(pd->d_name) != 0)
                    if (getProcessInfo(atoi(pd->d_name)))
                        continue;
        }

        closedir(proc_dir);

        calcCPUUsage();

        return true;
    }

    bool ProcMon::getProcessInfo(unsigned int pid) {
        // init temp variables
        char cur_stat = '\0';
        unsigned long parent_pid = 0;
        unsigned long used_user_time = 0;
        unsigned long used_system_time = 0;
        unsigned long start_time = 0;
        unsigned long vsz = 0;
        unsigned long rss = 0;
        unsigned long cpu_time;
        unsigned int thread_count = 0;
        std::string owner;
        std::string process_name;

        // get UID
        char stat_uid[BUFFERSIZE];
        struct stat stat_info;
        memset(stat_uid, 0, BUFFERSIZE);
        snprintf(stat_uid, BUFFERSIZE, SYS_PROC_LOC, pid);
        if (stat(stat_uid, &stat_info) == -1)
            return false;

        // get owner
        struct passwd *cur_pw;
        cur_pw = getpwuid(stat_info.st_uid);
        if (cur_pw == NULL) {
            char uid_str[BUFFERSIZE];
            memset(uid_str, 0, BUFFERSIZE);
            snprintf(uid_str, BUFFERSIZE, "%lu", stat_info.st_uid);
            owner = std::string(uid_str);
        } else
            owner = std::string(cur_pw->pw_name);

        // get process name
        char proc_name_buff[BUFFERSIZE];
        memset(proc_name_buff, 0, BUFFERSIZE);
        if (!(getProcessName(pid, proc_name_buff, BUFFERSIZE)))
            return false;
        process_name = proc_name_buff;

        // get other process information
        char buffer_proc[BUFFERSIZE];
        snprintf(buffer_proc, BUFFERSIZE, SYS_PROC_STAT, pid);
        FILE *proc_file = fopen(buffer_proc, "r");
        if (proc_file != NULL) {
            fscanf(proc_file, SYS_PROC_PATTERN,
                   &cur_stat,
                   &parent_pid,
                   &used_user_time,
                   &used_system_time,
                   &thread_count,
                   &start_time,
                   &vsz,
                   &rss);
            fclose(proc_file);
            proc_file = 0;
        }

        // get CPU time
        unsigned long cpu_time_jiffies = (used_system_time + used_user_time);
        if (cpu_time_jiffies > 0)
            cpu_time = (cpu_time_jiffies / HZ);

        // create and fill process info struct
        ProcessInfo new_proc;
        new_proc.pid = pid;
        new_proc.uid = stat_info.st_uid;
        new_proc.owner = owner;
        new_proc.ppid = parent_pid;
        new_proc.used_user_time = used_user_time;
        new_proc.used_system_time = used_system_time;
        new_proc.thread_count = thread_count;
        new_proc.name = process_name;
        new_proc.cpu_time = cpu_time;
        new_proc.process_status = cur_stat;
        if ((boot_time_ + start_time) > 0)
            new_proc.start_time = (boot_time_ + start_time) / HZ;
        else
            new_proc.start_time = 0;
        if (vsz > 0)
            new_proc.vsz = vsz / 1024;
        else
            new_proc.vsz = 0;
        if (rss > 0)
            new_proc.rss = rss * 4;

        // push new process struct to current buffer
        this->cur_buffer_.push_back(new_proc);

        return true;
    }

    bool ProcMon::getProcessName(unsigned int pid, char *buffer, const unsigned int size) {
        // get command line
        char stat_proc[BUFFERSIZE];
        FILE *ps_file = NULL;
        bool done = false;

        snprintf(stat_proc, BUFFERSIZE, SYS_PROC_CMD, pid);
        ps_file = fopen(stat_proc, "r");
        if (ps_file != 0) {
            char cmd_line[BUFFERSIZE];
            int read_size = 0;
            memset(cmd_line, 0, BUFFERSIZE);
            read_size = fread(cmd_line, 1, BUFFERSIZE, ps_file);
            fclose(ps_file);
            cmd_line[BUFFERSIZE - 1] = '\0';
            if (read_size != 0) {
                memcpy(buffer, cmd_line, size);
                done = true;
            }
        }
        return done;
    }

    bool ProcMon::getProcessNameByStat(unsigned int pid, char *buffer, const unsigned int size)
    {
        char stat_proc[BUFFERSIZE];
        FILE *ps_file = 0;
        bool done = false;

        snprintf(stat_proc, BUFFERSIZE, SYS_PROC_STAT, pid);
        ps_file = fopen(stat_proc, "r");
        if(ps_file != 0)
        {
            char cmd_line[BUFFERSIZE];
            int match_item = 0;
            memset(cmd_line, 0, BUFFERSIZE);

            // restrict maximum chars is 255, it could prevent security warning
            match_item = fscanf(ps_file, SYS_PROC_BIN, &pid, cmd_line);
            fclose(ps_file);

            if(match_item == 2)
            {
                cmd_line[BUFFERSIZE - 1] = '\0';

                // remove ')'
                if(cmd_line[strlen(cmd_line) - 1] == ')')
                    cmd_line[strlen(cmd_line) - 1] = '\0';

                memcpy(buffer, cmd_line, size);
                done = true;
            }
        }
        return done;
    }

    void ProcMon::calcCPUUsage() {
        float cpu_usage = 0;

        if (this->prev_buffer_.empty())
            return;

        float cur_cpu_time = (float) cpu_info_.getCPUTime();
        //if(cur_cpu_time == 0)
        //    return;

        std::list<ProcessInfo>::iterator cur_ite = cur_buffer_.begin();
        std::list<ProcessInfo>::iterator prev_ite = prev_buffer_.begin();
        // calculate load for each process
        do {
            if (cur_ite->pid != prev_ite->pid) {
                cur_ite->cpu_usage = cpu_usage;
                ++prev_ite;
                ++cur_ite;
                continue;
            }

            cur_cpu_time = (float) cpu_info_.getCPUTime();

            unsigned long proc_cpu_time = 0;
            proc_cpu_time += cur_ite->used_system_time - prev_ite->used_system_time;
            proc_cpu_time += cur_ite->used_user_time - prev_ite->used_user_time;


            if (proc_cpu_time != 0) {
                cpu_usage = ((float) (proc_cpu_time * 100) / cur_cpu_time);
            }

            cur_ite->cpu_usage = cpu_usage;
            ++prev_ite;
            ++cur_ite;
            cpu_usage = 0;

        } while (cur_ite != cur_buffer_.end());

        return;
    }

    bool ProcMon::refresh() {
        // refresh uptime
        this->getBootTime();

        // clean up and gather process information
        prepare();
        if(this->gatherProcesses())
            finish();
        return false;
    }

    void ProcMon::prepare() {
        // clean up
        if (!this->prev_buffer_.empty())
            this->prev_buffer_.clear();

        // move current to previous
        this->prev_buffer_.assign(cur_buffer_.begin(), cur_buffer_.end());
        this->cur_buffer_.clear();
    }

    void ProcMon::finish()
    {
        // clean up process vector
        if(!this->process_list_.empty())
            this->process_list_.clear();

        // move current buffer to process vector
        this->process_list_.reserve(this->cur_buffer_.size());
        std:copy(std::begin(this->cur_buffer_), std::end(this->cur_buffer_),
                 std::back_inserter(this->process_list_));
    }

    void ProcMon::getBootTime() {
        // get uptime
        unsigned long uptime = 0;
        FILE *uptime_file = fopen(SYS_BOOT_TIME, "r");

        if(uptime_file)
        {
            if (1 != fscanf(uptime_file, "%lu.%*u", &uptime))
                uptime = 0;
            fclose(uptime_file);
        }

        time_t cur_time = time(0);

        this->boot_time_ = cur_time - uptime;
    }

    std::vector<ProcessInfo> ProcMon::getData()
    {
        std::vector<ProcessInfo> cp_buffer;
        cp_buffer.reserve(this->cur_buffer_.size());
        std:copy(std::begin(this->cur_buffer_), std::end(this->cur_buffer_),
                 std::back_inserter(cp_buffer));
        return cp_buffer;
    }

}
