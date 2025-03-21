#ifndef EDF_H
#define EDF_H
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "task.h"
#include "input.h"

void simulateEDF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time);

#endif //EDF_H