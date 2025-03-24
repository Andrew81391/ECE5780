#include "llf.h"
using namespace std;

void simulateLLF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time){
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
		// Find the task with the leasat laxity that's ready to run
		for (uint32_t i = 0; i < all_tasks.size(); i++) {
			auto& task = all_tasks[i];
			// Skip finished aperiodic tasks
			if (!task.is_periodic && task.progress_left <= 0) continue;
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
			cout << "[" << sys_time << " ms] Preempted task: " << all_tasks[index_of_current_task].ID << endl;
		}
		// Log start of new task
		if (least_index != -1 && index_of_current_task != least_index) {
			cout << "[" << sys_time << " ms] Starting task: " << all_tasks[least_index].ID << endl;
		}
		index_of_current_task = least_index;
		// Execute the selected task
		if (index_of_current_task != -1) {
			auto& task = all_tasks[index_of_current_task];
			task.progress_left--;
			if (task.progress_left == 0) {
				cout << "[" << sys_time << " ms] Finished task: " << task.ID << endl;
				task_finished = true;
			} else {
				task_finished = false;
			}
		} else {
			cout << "[" << sys_time << " ms] CPU Idle\n";
		}
		// Check for deadline misses
		for (auto& task : all_tasks) {//check each task
			if (sys_time == task.next_deadline - 1) {//
				if (task.progress_left > 0) {
					task.missed_deadlines++;
					task_finished = true;
					cout << "[" << sys_time << " ms] Missed deadline: " << task.ID << endl;
				}
				if (task.is_periodic) {
					// reset task
					task.progress_left = task.exec_time;
					task.next_deadline += task.period;
				} else {
					task.progress_left = 0;
				}
			}
		}
	}
}