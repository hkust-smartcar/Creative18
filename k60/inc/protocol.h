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

	Protocol():m_bt(1,1), test(nullptr), pWheelbase(nullptr){
		Init();
	}

	Protocol(Test* t):m_bt(1,1), test(t), pWheelbase(nullptr){
		Init();
	}

	Protocol(Wheelbase* wb):m_bt(1,1), test(nullptr), pWheelbase(wb){
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
	int32_t AwaitRequestEncoder(LcdTypewriter* t = nullptr);

	void Handler(const Bluetooth::Package& pkg);

	void RequestSetMotorHandler(const Bluetooth::Package& pkg);
	void RequestEncoderHandler(const Bluetooth::Package& pkg);
	void ResponseEncoderHandler(const Bluetooth::Package& pkg);

	int32_t GetEncoderTotolCount(){return encoder_totoal_count;}
	int32_t GetEncoderCount(){return encoder_count;}

private:
	bool recievedPackageId[10];
	uint8_t filteredRecievedPackageSumByType[10];

	bool waiting_encoder = false;
	int32_t encoder_count = 0;
	int32_t encoder_totoal_count = 0;
};



#endif /* INC_PROTOCOL_H_ */
