/*
 * protocol.h
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <string>
#include <map>

#include "libsc/lcd_typewriter.h"

#include "scheduler.h"
#include "bluetooth.h"
#include "test.h"
#include "wheelbase.h"

using namespace libsc;

class Test;

class Wheelbase;

class Protocol{
public:

	Bluetooth m_bt;
	Test* test;
	Wheelbase* pWheelbase;
	Scheduler* pScheduler;

	Protocol(Scheduler* pScheduler):m_bt(1,pScheduler), test(nullptr), pWheelbase(nullptr), pScheduler(pScheduler){
		Init();
	}

	Protocol(Scheduler* pScheduler, Test* t):m_bt(1,pScheduler), test(t), pWheelbase(nullptr){
		Init();
	}

	Protocol(Scheduler* pScheduler, Wheelbase* wb):m_bt(1,pScheduler), test(nullptr), pWheelbase(wb){
		Init();
	}

	void Init(){
		m_bt.SetCustomHandler([&](const Bluetooth::Package& pkg){
			this->Handler(pkg);
		});
		memset(recievedPackageId,0,10);
		memset(filteredRecievedPackageSumByType,0,10);
	}

	void SetResendPeriod(time_t period){
		m_bt.SetResendPeriod(period);
	}

	/**
	 * Requests and Responses:
	 *
	 * Return 0 means submit request fail,
	 * probably previous same request have not received
	 */

	uint8_t RequestSetMotor(int16_t speed);

	//request encoder using the request encoder, the feedback will provided by ResponseEncoderHandler
	//when requested, use the response encoder function
	uint8_t RequestEncoder();
	uint8_t ResponseEncoder(int32_t count);
	uint8_t ResponseEncoderById(uint8_t id, int32_t count);
	uint8_t RequestSetServo(uint16_t degree);
	uint8_t RequestAutoFeedEncoders(uint16_t interval);
	int32_t AwaitRequestEncoder(LcdTypewriter* t = nullptr);

	void Handler(const Bluetooth::Package& pkg);

	void RequestSetMotorHandler(const Bluetooth::Package& pkg);
	void RequestEncoderHandler(const Bluetooth::Package& pkg);
	void ResponseEncoderHandler(const Bluetooth::Package& pkg);
	void ResponseEncoderByIdHandler(const Bluetooth::Package& pkg);
	void RequestSetServoHandler(const Bluetooth::Package& pkg);
	void RequestAutoFeedEncodersHandler(const Bluetooth::Package& pkg);

	int32_t GetEncoderCount(){return encoder_count;}

private:
	bool recievedPackageId[10];
	uint8_t filteredRecievedPackageSumByType[10];
	uint16_t auto_feed_encoder_job_id = -1;
	int32_t encoder_img = 0;

	bool waiting_encoder = false;
	int32_t encoder_count = 0;
};



#endif /* INC_PROTOCOL_H_ */
