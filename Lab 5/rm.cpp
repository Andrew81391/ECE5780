#include "rm.h"
using namespace std;

void simulateRM(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time, ofstream& outfile){
    outfile << "----------------------" << endl << "----Rate Monotonic Scheduler----" 
        << endl << "----------------------" << endl;
    int32_t sys_time=0;
	//combine all tasks into a single vector
	std::vector<taskPeriodic> all_tasks = taskP;
	all_tasks.insert(all_tasks.end(), taskNP.begin(), taskNP.end());
	//we need to keep track of the currently running task.
	int32_t index_of_current_task=-1;
	bool task_finished=false;
    

	
	//loop once per ms(basic time unit)
	for (sys_time = 0; sys_time < run_time; sys_time++) {
		int32_t highest_priority_index = -1;
		int32_t highest_priority_period = INT32_MAX;
		//find the task with shortest period that's ready to run
        //only look in the periodic tasks here
		for (uint32_t i = 0; i < (taskP.size()); i++) {
			auto& task = all_tasks[i];
			// Skip finished aperiodic tasks
			if (!task.is_periodic && task.progress_left <= 0) continue;
            //check for a shorter period and set it to the highest priority task
			if (task.is_periodic) {
				if (task.period < highest_priority_period && (task.progress_left > 0)) {
					highest_priority_period = task.period;
					highest_priority_index = i;
				}
			}
		}
        //if there were no available periodic tasks, check for available aperiodic tasks
        if (highest_priority_index == -1){
            //iterate through the tasks to find the highest priority (by release time) aperiodic task
            int32_t highest_priority_index_aperiodic = -1;
            int32_t highest_priority_release_time_aperiodic = INT32_MAX;
            //this loop finds the highest priority aperiodic task that can run
            for (uint32_t i = 0; i<all_tasks.size(); i++){
                auto& task = all_tasks.at(i);
                //Only consider released, unfinished aperiodic tasks
                if ((!task.is_periodic) && (sys_time>=task.release_time) && (task.progress_left>0)){
                    if (task.release_time < highest_priority_release_time_aperiodic){
                        highest_priority_index_aperiodic = i;
                        highest_priority_release_time_aperiodic = task.release_time;
                    }
                }
            }//after this, highest_priority_index_aperiodic== -1 if there are no available
            //aperiodic tasks to run, and the index of the task if there are avalable tasks
            if (highest_priority_index_aperiodic != -1){//available aperiodic tasks to run
                //new task to run
                highest_priority_index=highest_priority_index_aperiodic;
            }
        }
        

		// Log preemption
		if (index_of_current_task != -1 && (index_of_current_task != highest_priority_index) && (!task_finished)) {
			all_tasks[index_of_current_task].preemptions++;
			outfile << "[" << sys_time << " ms] Preempted task: " << all_tasks[index_of_current_task].ID << endl;
		}
		// Log start of new task
		if (highest_priority_index != -1 && index_of_current_task != highest_priority_index) {
			outfile << "[" << sys_time << " ms] Starting task: " << all_tasks[highest_priority_index].ID << endl;
		}
		index_of_current_task = highest_priority_index;
		// Execute the selected task
		if (index_of_current_task != -1) {
			auto& task = all_tasks[index_of_current_task];
			task.progress_left--;
			//did the task finish
			if (task.progress_left == 0) {
				outfile << "[" << sys_time << " ms] Finished task: " << task.ID << endl;
                //log the time finished for aperiodic tasks
				if (!task.is_periodic) {
					task.time_finished = sys_time;
				}
                //set finished flag for preemption logging
				task_finished=true;
			} else {
				task_finished=false;
			}
        //nothing to run
		} else {
			outfile << "[" << sys_time << " ms] CPU Idle\n";
		}
		// Check for end of period to update tasks
		for (auto& task : all_tasks) {//check each task
			if (sys_time == (task.next_deadline-1)) {
				//check for missed deadline
				if (task.progress_left > 0) {
					task.missed_deadlines++;
					//'kill' late tasks
					task_finished=true;
					outfile << "[" << sys_time << " ms] Missed deadline: " << task.ID << endl;
					//mark completion time foor aperiodic tasks to calculate latency
					if (!task.is_periodic) {
						task.time_finished = sys_time;
					}
				}
                //task completed, but is periodic so runs again
				if (task.is_periodic){
					//reset task, stops task from running multiple times in its period.
					task.progress_left = task.exec_time;
					task.next_deadline += task.period;
				} 
                //for aperodic tasks missing deadline=death
				else {
					task.progress_left=0;
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