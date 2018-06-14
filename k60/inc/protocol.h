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
#include "libsc/k60/touchscreen_lcd.h"

#include "bluetooth.h"
#include "test.h"

class Test;

class Protocol{
public:

	Bluetooth m_bt;
	Test* test;

	Protocol():m_bt(0,1), test(nullptr){
		Init();
	}

	Protocol(Test* t):m_bt(0,1), test(t){
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
	uint8_t ResponseEncoder(uint16_t count);

	void Handler(const Bluetooth::Package& pkg);

	void RequestSetMotorHandler(const Bluetooth::Package& pkg);
	void RequestEncoderHandler(const Bluetooth::Package& pkg);
	void ResponseEncoderHandler(const Bluetooth::Package& pkg);

private:
	bool recievedPackageId[10];
	uint8_t filteredRecievedPackageSumByType[10];
};



#endif /* INC_PROTOCOL_H_ */
