#include "llf.h"
using namespace std;

void simulateLLF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time, ofstream& outfile){
	outfile << "----------------------" << endl << "----Least Laxity First----" 
        << endl << "----------------------" << endl;
	int32_t sys_time=0;
	//combine all tasks into a single vector, we can treat periodic and aperiodic
	//tasks *exactly the same for this scheduler
	std::vector<taskPeriodic> all_tasks = taskP;
	all_tasks.insert(all_tasks.end(), taskNP.begin(), taskNP.end());
	//we need to keep track of the currently running task.
	int32_t index_of_current_task=-1;
	bool task_finished=false;
	
	//loop once per ms(basic time unit)
	for (sys_time = 0; sys_time < run_time; sys_time++) {
		int32_t least_index = -1;
		int32_t least_laxity = INT32_MAX;
		// Find the task with the least laxity that's ready to run
		for (uint32_t i = 0; i < all_tasks.size(); i++) {
			auto& task = all_tasks[i];
			// Skip finished aperiodic tasks
			if (!task.is_periodic && task.progress_left <= 0) continue;
			//check for a task with less slack
			if (task.is_periodic || (task.release_time <= sys_time)) {
				int32_t laxity = task.next_deadline - sys_time - task.progress_left;
				if (laxity < least_laxity && task.progress_left > 0) {
					least_laxity = laxity;
					least_index = i;
				}
			}
		}
		// Log preemption
		if (index_of_current_task != -1 && (index_of_current_task != least_index) && (!task_finished)) {
			all_tasks[index_of_current_task].preemptions++;
			outfile << "[" << sys_time << " ms] Preempted task: " << all_tasks[index_of_current_task].ID << endl;
		}
		// Log start of new task
		if (least_index != -1 && index_of_current_task != least_index) {
			outfile << "[" << sys_time << " ms] Starting task: " << all_tasks[least_index].ID << endl;
		}
		index_of_current_task = least_index;
		// Execute the selected task
		if (index_of_current_task != -1) {
			auto& task = all_tasks[index_of_current_task];
			task.progress_left--;
			//did the task finish
			if (task.progress_left == 0) {
				outfile << "[" << sys_time << " ms] Finished task: " << task.ID << endl;
				//log time finished for aperiodic tasks
				if (!task.is_periodic) {
					task.time_finished = sys_time;
				}
				//set time finished flag for preemption logging
				task_finished = true;
			} else {
				task_finished = false;
			}
		//nothing to run
		} else {
			outfile << "[" << sys_time << " ms] CPU Idle\n";
		}
		// Check for deadline misses
		for (auto& task : all_tasks) {//check each task
			if (sys_time == task.next_deadline - 1) {//
				if (task.progress_left > 0) {
					task.missed_deadlines++;
					//'kill' late tasks
					task_finished = true;
					outfile << "[" << sys_time << " ms] Missed deadline: " << task.ID << endl;
					//mark time of completion for aperiodic tasks to calculate latency
					if (!task.is_periodic) {
						task.time_finished = sys_time;
					}
				}
				if (task.is_periodic) {
					// reset task
					task.progress_left = task.exec_time;
					task.next_deadline += task.period;
				//kill aperiod tasks at their deadline
				} else {
					task.progress_left = 0;
				}
			}
		}
	}

	//print the summary in a table

	int32_t total_aperiodic_time = 0;

    outfile << endl;

    //create column titles
    outfile << left << setw(8) << "Task" 
         << setw(15) << "Preemptions" 
         << setw(20) << "Missed Deadlines" 
         << setw(20) << "Response Time (ms)" 
         << endl;

    //separate titles from data
    outfile << string(63, '-') << endl;

    //go through each task
    for (const auto& task : all_tasks) {
        //print id, preemptions, and missed deadlines for all tasks
        outfile << left << setw(8) << task.ID 
             << setw(15) << task.preemptions 
             << setw(20) << task.missed_deadlines;
        //print response time for aperiodic tasks
        if (!task.is_periodic) {
            int response_time = task.time_finished - task.release_time;
            total_aperiodic_time += response_time;
            outfile << setw(20) << response_time;
        //no response time for periodic tasks
        } else {
            outfile << setw(20) << "-";
        }
        outfile << endl;
    }

    //if there are aperiodic tasks, print the average response time
    if (taskNP.size() > 0) {
        double avg_response = (double)total_aperiodic_time / (double)taskNP.size();
        outfile << "\nAverage aperiodic response time: " << fixed << setprecision(2) << avg_response << " ms";
    }

    outfile << endl << endl;
}