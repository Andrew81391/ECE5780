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
    std::string ID;
    int32_t exec_time;
    int32_t period;        // Set to 0 if aperiodic
    int32_t release_time;  // Set to 0 if periodic
    bool is_periodic;
	int32_t missed_deadlines;
	int32_t preemptions;
	int32_t next_deadline;

    taskPeriodic(std::string id, int32_t e, int32_t p, int32_t r, bool periodic);
};

#endif // TASK_PERIODIC_H
