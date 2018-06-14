/*
 * bluetooth.h
 *
 *  Created on: Mar 2, 2018
 *      Author: dipsy
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "libsc/k60/jy_mcu_bt_106.h"
#include "libbase/k60/pit.h"
#include "libsc/k60/uart_device.h"
#include <libsc/system.h>

#include "comm.h"

using libsc::System;
using libsc::k60::JyMcuBt106;
using libbase::k60::Pit;

class Bluetooth:public Comm{
public:
	Bluetooth(int bt_id, int pit_channel);
	~Bluetooth(){}

	void SendBuffer(const Byte* data, const size_t& size){
		m_bt.SendBuffer(data,size);
	}

	void SetResendPeriod(time_t period){
		resend_period = period;
	}

private:
	JyMcuBt106 m_bt;
	Pit m_pit;

	/**
	 * resend request for this period in ms
	 */
	time_t resend_period = 10;
	time_t next_send = 0;

	JyMcuBt106::Config GetBtConfig(int bt_id){
		JyMcuBt106::Config config;
		config.id = bt_id;
		config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
		config.rx_isr = [&](const Byte* data, const size_t size){
			return Comm::Listener(data,size);
		};
		return config;
	}

	Pit::Config GetPitConfig(int pit_channel){
		Pit::Config config;
		config.channel = pit_channel;
		config.count = 75000*250;
		config.isr = [&](Pit*){
			if(System::Time()>next_send){
				next_send+=resend_period;
				Comm::Period();
			}
		};
		return config;
	}
};



#endif /* INC_BLUETOOTH_H_ */
