
#include "edf.h"

void simulateEDF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time){
	//combine all tasks into a single vector
	std::vector<taskPeriodic> all_tasks = periodic_tasks;
	all_tasks.insert(all_tasks.end(), aperiodic_tasks.begin(), aperiodic_tasks.end());

	
	//loop once per ms(basic time unit
	for (int sys_time=0; sys_time<run_time; sys_time++){
		//check which task has the earliest deadline.  Inefficient but I cannot be assed to do better right now
		taskPeriodic earliest_task = all_tasks.at(0);
		//find the earliest deadline task for this systime interval
		for (const auto& task : all_tasks) {
			if (task.is_periodic){
				
			} else {
				if ((task.next_deadline < earliest_task.next_deadline)){
					earliest_task = task;
				}
			}
		}
	}
}