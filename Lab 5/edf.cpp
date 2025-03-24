
#include "edf.h"
using namespace std;

void simulateEDF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time){
	int32_t sys_time=0;
	//combine all tasks into a single vector, we can treat periodic and aperiodic
	//tasks *exactly the same for this scheduler
	std::vector<taskPeriodic> all_tasks = taskP;
	all_tasks.insert(all_tasks.end(), taskNP.begin(), taskNP.end());
	//we need to keep track of the currently running task.
	int32_t index_of_current_task=-1;

	
	// //loop once per ms(basic time unit)
	// for (int sys_time=0; sys_time<run_time; sys_time++){
	// 	//check which task has the earliest deadline.  Inefficient but I cannot be assed to do better right now
	// 	int32_t earliest_index = -1;
	// 	int32_t earliest_deadline = INT32_MAX;
	// 	//find the earliest deadline task for this systime interval
	// 	for (int32_t i = 0; i<all_tasks.size();i++){
	// 		//select a task.  use .at() for safety
	// 		const auto& task = all_tasks.at(i);
	// 		//check if task is valid for running
	// 		if (task.is_periodic || (task.release_time <= sys_time)){
	// 			//test if this task is more important
	// 			if (task.next_deadline < earliest_deadline){
	// 				//update values
	// 				earliest_deadline = task.next_deadline;
	// 				earliest_index = i;
	// 			}
	// 		}
	// 	}//earliest_index now has the index of the task w/ closest deadline
	//TODO: handle the switching of tasks based on the next priority task


	//TODO: handle the updating of next deadline values within a task
	for (sys_time = 0; sys_time < run_time; sys_time++) {
		int32_t earliest_index = -1;
		int32_t earliest_deadline = INT32_MAX;

		// Find the task with the earliest deadline that's ready to run
		for (int32_t i = 0; i < all_tasks.size(); i++) {
			auto& task = all_tasks[i];

			// Skip finished aperiodic tasks
			if (!task.is_periodic && task.progress_left <= 0) continue;

			// Periodic or released aperiodic
			if (task.is_periodic || (task.release_time <= sys_time)) {
				if (task.next_deadline < earliest_deadline && task.progress_left > 0) {
					earliest_deadline = task.next_deadline;
					earliest_index = i;
				}
			}
		}

		// Preemption detection
		if (index_of_current_task != -1 && index_of_current_task != earliest_index) {
			all_tasks[index_of_current_task].preemptions++;
		}
		index_of_current_task = earliest_index;

		// Execute the current task
		if (index_of_current_task != -1) {
			auto& task = all_tasks[index_of_current_task];
			task.progress_left--;

			// If the task just completed
			if (task.progress_left == 0) {
				if (task.is_periodic) {
					task.next_deadline += task.period;
					task.progress_left = task.exec_time;
				}
				// aperiodic stays at 0
			}
		}

		// Check for deadline misses
		for (auto& task : all_tasks) {
			if (task.is_periodic && sys_time == task.next_deadline) {
				if (task.progress_left > 0) {
					task.missed_deadlines++;
					// reset task
					task.progress_left = task.exec_time;
				}
				task.next_deadline += task.period;
			}
		}
	}

	// Optional: print stats
	for (const auto& task : all_tasks) {
		cout << task.ID << " | Missed Deadlines: " << task.missed_deadlines
		     << " | Preemptions: " << task.preemptions << endl;
	}

	}
}