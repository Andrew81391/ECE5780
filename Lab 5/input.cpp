#include "input.h"

//function to read each task from the input file
void parseTaskLine(const string& line, string& id, int32_t& first, int32_t& second) {
    istringstream ss(line);
    string token;

    getline(ss, token, ','); // ID
    id = token;
    getline(ss, token, ','); // Exec time
    first = stoi(token);
    getline(ss, token, ','); // Period or Release time
    second = stoi(token);
}

//function to read an input file into our task vectors
void readInputFile(const string& filename, vector<taskPeriodic>& periodic_tasks, vector<taskPeriodic>& aperiodic_tasks, int& sim_time) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    int num_tasks;
    infile >> num_tasks;    //read number of periodic tasks
    infile >> sim_time;     //read sim time
    string line;
    getline(infile, line); // eat newline

    // Read periodic tasks
    for (int i = 0; i < num_tasks; i++) {
        //read the task line
        getline(infile, line);
        string id;
        int exec_time, period;
        //parse line for task properties
        parseTaskLine(line, id, exec_time, period);
        //add task to vector
        periodic_tasks.emplace_back(id, exec_time, period, 0, true, -1);
    }

    //read aperiodic tasks
    int num_aperiodic_tasks = 0;
    if (getline(infile, line) && !line.empty() && isdigit(line[0])) {
        //get number of aperiodic tasks
        num_aperiodic_tasks = stoi(line);
        //for each aperiodic task
        for (int i = 0; i < num_aperiodic_tasks; i++) {
            //read the task line
            getline(infile, line);
            string id;
            int exec_time, release_time;
            //parse line for task properties
            parseTaskLine(line, id, exec_time, release_time);
            //add task to vector
            aperiodic_tasks.emplace_back(id, exec_time, 500, release_time, false, -1);
        }
    }
}