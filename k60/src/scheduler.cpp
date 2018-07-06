/*
 * scheduler.cpp
 *
 *  Created on: Jul 6, 2018
 *      Author: dipsy
 */

#include "scheduler.h"

Scheduler::Scheduler(uint8_t channel_id, uint32_t count):m_pit(GetPitConfig(channel_id, count)){
}

uint16_t Scheduler::SetInterval(function<void(void)> f, time_t interval){
	interval_jobs.push_back({++schedule_sum,System::Time()+interval,interval, f});
	return schedule_sum;
}

uint16_t Scheduler::SetTimeout(function<void(void)> f, time_t delay){
	timeout_jobs.push_back({++schedule_sum,System::Time()+delay, delay, f});
	return schedule_sum;
}

bool Scheduler::ClearInterval(uint16_t job_id){
	for(int i = 0; i < interval_jobs.size(); i++){
		if(interval_jobs[i].id == job_id){
			interval_jobs.erase(interval_jobs.begin()+job_id);
			return true;
		}
	}
	return false;
}

bool Scheduler::ClearTimeout(uint16_t job_id){
	for(int i = 0; i < timeout_jobs.size(); i++){
		if(timeout_jobs[i].id == job_id){
			timeout_jobs.erase(timeout_jobs.begin()+job_id);
			return true;
		}
	}
	return false;
}

void Scheduler::Peroid(){
	time_t now = System::Time();
	for(int i = 0; i < interval_jobs.size(); ++i){
		if(now > interval_jobs[i].threshold){
			interval_jobs[i].threshold = now + interval_jobs[i].delay;
			interval_jobs[i].job();
		}
	}
	for(int i = timeout_jobs.size(); i >=0; --i){
		if(now > timeout_jobs[i].threshold){
			timeout_jobs[i].job();
			ClearTimeout(i);
		}
	}
}
