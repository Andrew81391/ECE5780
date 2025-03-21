#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

class taskPeriodic {
public:
    string ID;
    int32_t exec_time;
    int32_t period;        // set to 0 if aperiodic
    int32_t release_time;  // set to 0 if periodic
    bool is_periodic;

    taskPeriodic(string id, int32_t e, int32_t p, int32_t r, bool periodic)
        : ID(id), exec_time(e), period(p), release_time(r), is_periodic(periodic) {}
};

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
            aperiodic_tasks.emplace_back(id, exec_time, 0, release_time, false);
        }
    }
}

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

    return 0;
}
