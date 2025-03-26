#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "task.h"
#include "input.h"
#include "edf.h"
#include "llf.h"
#include "rm.h"


using namespace std;



int main(int argc, char* argv[]) {
    //check args
    if (argc != 3) {
        cerr << "Incorrect Argument Count" << endl;
        return 1;
    }

    //create task vectors
    vector<taskPeriodic> periodic_tasks;
    vector<taskPeriodic> aperiodic_tasks;
    int sim_time = 0;

    //read the input file into the task vectors
    readInputFile(argv[1], periodic_tasks, aperiodic_tasks, sim_time);

    //open outfile
    ofstream outfile(argv[2]);
    if (!outfile) {
        cerr << "Error opening output file: " << argv[2] << endl;
    }

    //print info about the task set
    outfile << "Simulation Time: " << sim_time << " ms\n";

    outfile << "Periodic Tasks:\n";
    for (const auto& task : periodic_tasks) {
        outfile << task.ID << ": Exec Time = " << task.exec_time << ", Period = " << task.period << "\n";
    }

    outfile << "Aperiodic Tasks:\n";
    for (const auto& task : aperiodic_tasks) {
        outfile << task.ID << ": Exec Time = " << task.exec_time << ", Release Time = " << task.release_time << "\n";
    }
	
	
	//Solve for RM
	simulateRM(periodic_tasks, aperiodic_tasks, sim_time, outfile);
	//Solve for EDF
	simulateEDF(periodic_tasks, aperiodic_tasks, sim_time, outfile);
	//Solve for LLF
	simulateLLF(periodic_tasks, aperiodic_tasks, sim_time, outfile);
	
    outfile.close();

    return 0;
}
