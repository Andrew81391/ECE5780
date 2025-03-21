
#include "llf.h"

void simulateLLF(std::vector<taskPeriodic> taskP, std::vector<taskPeriodic> taskNP, int32_t run_time){
	int32_t sys_time=0;
	//combine all tasks into a single vector
	std::vector<taskPeriodic> all_tasks = taskP;
	all_tasks.insert(all_tasks.end(), taskNP.begin(), taskNP.end());

	
	//loop once per ms(basic time unit
	for (int sys_time=0; sys_time<run_time; sys_time++){
		//check which task has the earliest deadline.  Inefficient but I cannot be assed to do better right now
		taskPeriodic earliest_task = all_tasks.at(0);
		//find the earliest deadline task for this systime interval
		
	}
}