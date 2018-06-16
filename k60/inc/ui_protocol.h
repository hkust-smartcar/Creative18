/*
 * ui_protocol.h
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#ifndef INC_UIPROTOCOL_H_
#define INC_UIPROTOCOL_H_

#include <string>
#include <map>
#include "libsc/k60/touchscreen_lcd.h"

#include "bluetooth.h"
#include "test.h"
#include "wheelbase.h"

class Test;

class Wheelbase;

class UiProtocol{
public:

	Bluetooth m_bt;
	Test* test;
	Wheelbase* pWheelbase;

	UiProtocol():m_bt(0,2), test(nullptr){
		Init();
	}

	UiProtocol(Test* t):m_bt(0,2), test(t){
		Init();
	}

	UiProtocol(Wheelbase* wb):m_bt(0,2), pWheelbase(wb){
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

	uint8_t RequestMove(int16_t speedx, int16_t speedy);

	//request encoder using the request encoder, the feedback will provided by ResponseEncoderHandler
	//when requested, use the response encoder function
	uint8_t RequestSetMotorById(uint8_t id, int16_t speed);

	void Handler(const Bluetooth::Package& pkg);

	void RequestMoveHandler(const Bluetooth::Package& pkg);
	void RequestSetMotorByIdHandler(const Bluetooth::Package& pkg);

private:
	bool recievedPackageId[10];
	uint8_t filteredRecievedPackageSumByType[10];
};



#endif /* INC_UIPROTOCOL_H_ */
