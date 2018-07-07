/*
 * bluetooth.cpp
 *
 *  Created on: Mar 2, 2018
 *      Author: dipsy
 */

#include "bluetooth.h"

Bluetooth::Bluetooth(int bt_id, Scheduler* pScheduler):
Comm(),m_bt(GetBtConfig(bt_id)),pScheduler(pScheduler){
	job_id = pScheduler->SetInterval([this]{
			Comm::Period();
	}, 10);
}

void Bluetooth::SetResendPeriod(time_t period){
	pScheduler->ClearInterval(job_id);
	job_id = pScheduler->SetInterval([this]{
			Comm::Period();
	}, period);
}
