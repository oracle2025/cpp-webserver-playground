#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#else
#include <stdio.h>
#include <sys/resource.h>
#endif

int check_memory_usage()
{
#if defined(__APPLE__) && defined(__MACH__)

    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (KERN_SUCCESS
        != task_info(
            mach_task_self(),
            TASK_BASIC_INFO,
            (task_info_t)&t_info,
            &t_info_count)) {
        return -1;
    }
    return t_info.resident_size;
    // resident size is in t_info.resident_size;
    // virtual size is in t_info.virtual_size;
#else
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage );
    return (size_t)(rusage.ru_maxrss * 1024L);
#endif
    return -1;
}