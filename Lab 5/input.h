#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "task.h"
using namespace std;

void parseTaskLine(const string& line, string& id, int32_t& first, int32_t& second);
void readInputFile(const string& filename, vector<taskPeriodic>& periodic_tasks, vector<taskPeriodic>& aperiodic_tasks, int& sim_time);

#endif //INPUT_H