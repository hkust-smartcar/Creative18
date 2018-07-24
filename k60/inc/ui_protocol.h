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
	Scheduler* pScheduler;

	UiProtocol(uint8_t id, Scheduler* pScheduler):m_bt(id,pScheduler), test(nullptr), pWheelbase(nullptr), pScheduler(pScheduler){
		Init();
	}

	UiProtocol(uint8_t id, Scheduler* pScheduler, Test* t):m_bt(id,pScheduler), test(t), pWheelbase(nullptr){
		Init();
	}

	UiProtocol(uint8_t id, Scheduler* pScheduler, Wheelbase* wb):m_bt(id,pScheduler), test(nullptr), pWheelbase(wb){
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
	uint8_t RequestSetMotorById(uint8_t id, int16_t speed);
	uint8_t ResponseEncoderById(uint8_t id, int32_t count);
	uint8_t ResponseEncoders(int32_t count0, int32_t count1, int32_t count2);

	void Handler(const Bluetooth::Package& pkg);

	void RequestMoveHandler(const Bluetooth::Package& pkg);
	void RequestSetMotorByIdHandler(const Bluetooth::Package& pkg);
	void RequestEncoderByIdHandler(const Bluetooth::Package& pkg);
	void RequestEncodersHandler(const Bluetooth::Package& pkg);
	void FeedGlobalRotationHandler(const Bluetooth::Package& pkg);
	void FeedLocalRotationHandler(const Bluetooth::Package& pkg);
	void FeedGlobalTranslationHandler(const Bluetooth::Package& pkg);
	void FeedLocalTranslationHandler(const Bluetooth::Package& pkg);
	void RequestAutoFeedEncodersHandler(const Bluetooth::Package& pkg);

private:
	bool recievedPackageId[10];
	uint8_t filteredRecievedPackageSumByType[10];
	uint16_t auto_feed_encoder_job_id = -1;
	int32_t encoder_img[3] = {0,0,0};
};



#endif /* INC_UIPROTOCOL_H_ */
