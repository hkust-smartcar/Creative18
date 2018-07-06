/*
 * scheduler.h
 *
 *  Created on: Jul 6, 2018
 *      Author: dipsy
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <vector>
#include <functional>
#include <libbase/k60/pit.h>
#include <libsc/system.h>

using std::vector;
using std::function;
using libsc::System;
using libbase::k60::Pit;

class Scheduler{
public:
	Scheduler();
	Scheduler(uint8_t channel_id, uint32_t count);

	/**
	 * brief: function $f will be called for every $interval ms, return $job_id
	 * param: {f} function to be called for every interval
	 * param: {interval} time interval for f to be called
	 * return: job_id assigned for f
	 */
	uint16_t SetInterval(function<void(void)> f, time_t interval);

	/**
	 * brief: function $f will be called after $delay ms, return $job_id
	 * param: {f} function to be called for after some time
	 * param: {delay} time delay for f to be called
	 * return: job_id assigned for f
	 *
	 */
	uint16_t SetTimeout(function<void(void)> f, time_t delay);

	/**
	 * brief: cancel job_id, return success or not
	 * param: {job_id} interval job to be cleared
	 * return: success or not
	 */
	bool ClearInterval(uint16_t job_id);

	/**
	 * brief: cancel job_id, return success or not
	 * param: {job_id} timeout job to be cleared
	 * return: success or not
	 */
	bool ClearTimeout(uint16_t job_id);

	/**
	 * brief: to be called peroidically, and perform the interval and timeout jobs
	 */
	void Peroid();
private:

	typedef struct{
		uint16_t id;			//job id
		time_t threshold;		//will call at this time
		time_t delay;			//call interval or delay time
		function<void()> job;	//the job
	} Job;

	uint16_t schedule_sum = 0;
	vector<Job> interval_jobs;
	vector<Job> timeout_jobs;

	Pit m_pit;

	Pit::Config GetPitConfig(uint8_t pit_channel, uint32_t count){
		Pit::Config config;
		config.channel = pit_channel;
		config.count = count;
		config.isr = [&](Pit*){
			Peroid();
		};
		return config;
	}
};



#endif /* INC_SCHEDULER_H_ */
