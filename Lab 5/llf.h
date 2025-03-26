#ifndef LLF_H
#define LLF_H
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "task.h"
#include "input.h"

void simulateLLF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time, ofstream& outfile);

#endif //LLF_H