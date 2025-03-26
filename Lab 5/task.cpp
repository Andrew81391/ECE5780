#include "task.h"

taskPeriodic::taskPeriodic(std::string id, int32_t e, int32_t p, int32_t r, bool periodic, int32_t sys_time_finished)
    : ID(id), exec_time(e), period(p), release_time(r), is_periodic(periodic), missed_deadlines(0), preemptions(0), next_deadline(p), progress_left(e), time_finished(sys_time_finished) {}
