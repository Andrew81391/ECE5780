#include "input.h"

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

void readInputFile(const string& filename, vector<taskPeriodic>& periodic_tasks, vector<taskPeriodic>& aperiodic_tasks, int& sim_time) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    int num_tasks;
    infile >> num_tasks;
    infile >> sim_time;
    string line;
    getline(infile, line); // eat newline

    // Read periodic tasks
    for (int i = 0; i < num_tasks; i++) {
        getline(infile, line);
        string id;
        int exec_time, period;
        parseTaskLine(line, id, exec_time, period);
        periodic_tasks.emplace_back(id, exec_time, period, 0, true);
    }

    int num_aperiodic_tasks = 0;
    if (getline(infile, line) && !line.empty() && isdigit(line[0])) {
        num_aperiodic_tasks = stoi(line);
        for (int i = 0; i < num_aperiodic_tasks; i++) {
            getline(infile, line);
            string id;
            int exec_time, release_time;
            parseTaskLine(line, id, exec_time, release_time);
            aperiodic_tasks.emplace_back(id, exec_time, 500, release_time, false);
        }
    }
}