#ifndef PROCESS_H
#define PROCESS_H

#include <string>

struct ProcessInfo {
    
    std::string      name;
    std::string      owner;

    unsigned long    uid;
    unsigned long    pid;
    unsigned long    rss;
    unsigned long    vsz;
    unsigned long    ppid;
    unsigned long    process_status;
    unsigned long    priority_level;
    unsigned long    used_user_time;
    unsigned long    used_system_time;
    unsigned long    start_time;
    unsigned long    cpu_time;
    unsigned int     thread_count;

    float            cpu_usage;

};

#endif