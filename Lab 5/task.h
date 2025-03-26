#ifndef TASK_PERIODIC_H
#define TASK_PERIODIC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

//this is class in not necessarily limited to periodic tasks
class taskPeriodic {
public:
    string ID;
    int32_t exec_time;
    int32_t period;        // Set to 500 if aperiodic
    int32_t release_time;  // Set to 0 if periodic
    bool is_periodic;
	int32_t missed_deadlines;
	int32_t preemptions;
	int32_t next_deadline;
    int32_t progress_left;  //leave at zero for a completed aperiodic task
    int32_t time_finished;

    taskPeriodic(string id, int32_t e, int32_t p, int32_t r, bool periodic, int32_t sys_time_finished);
};

#endif // TASK_PERIODIC_H
