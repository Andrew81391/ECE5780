#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "task.h"
#include "input.h"
#include "edf.h"
#include "llf.h"


using namespace std;



int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    vector<taskPeriodic> periodic_tasks;
    vector<taskPeriodic> aperiodic_tasks;
    int sim_time = 0;

    readInputFile(argv[1], periodic_tasks, aperiodic_tasks, sim_time);

    cout << "Simulation Time: " << sim_time << " ms\n";

    cout << "Periodic Tasks:\n";
    for (const auto& task : periodic_tasks) {
        cout << task.ID << ": Exec Time = " << task.exec_time << ", Period = " << task.period << "\n";
    }

    cout << "Aperiodic Tasks:\n";
    for (const auto& task : aperiodic_tasks) {
        cout << task.ID << ": Exec Time = " << task.exec_time << ", Release Time = " << task.release_time << "\n";
    }
	
	
	//Solve for RM
	
	//Solve for EDF
	//simulateEDF(periodic_tasks, aperiodic_tasks, sim_time);
	//Solve for LLF
	simulateLLF(periodic_tasks, aperiodic_tasks, sim_time);
	
    return 0;
}
