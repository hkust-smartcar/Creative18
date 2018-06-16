/*
 * ui_protocol.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#include "ui_protocol.h"

void UiProtocol::Handler(const Bluetooth::Package& pkg){
	Byte debug[100];
	memset(debug,0,100);
	memcpy(debug,&*pkg.data.begin(),pkg.data.size());
	if(test){
//		char c[20];
//		sprintf(c,"sum %d %d", m_bt.receive_package_count, m_bt.bytecount);
//		test->lcd.SetRegion({0,100,100,20});
//		test->writer.WriteString(c);
	}
	if(pkg.type != Bluetooth::PkgType::kACK){
		if(recievedPackageId[pkg.id%10]){
			return;
		}
		recievedPackageId[pkg.id%10] = true;
		recievedPackageId[(pkg.id+5)%10] = false;
	}
	filteredRecievedPackageSumByType[pkg.type]++;
	switch(pkg.type){
	case Bluetooth::PkgType::kRequestMove:
		RequestMoveHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestSetMotorById:
		RequestSetMotorByIdHandler(pkg);
		break;

	}
}

uint8_t UiProtocol::RequestMove(int16_t speedx, int16_t speedy){
	vector<Byte> data(4,0);
	memcpy(&*data.begin(),&speedx,2);
	memcpy(&*data.begin() + 2, &speedy,2);
	return m_bt.QueuePackage({Bluetooth::PkgType::kRequestMove,0,data});
}

uint8_t UiProtocol::RequestSetMotorById(uint8_t id, int16_t speed){
	vector<Byte> data(3,0);
	memcpy(&*data.begin(), &id, 1);
	memcpy(&*data.begin() + 1, &speed,2);
	return m_bt.QueuePackage({Bluetooth::PkgType::kRequestSetMotorById,0,{}});
}

void UiProtocol::RequestMoveHandler(const Bluetooth::Package& pkg){
	int16_t speedx, speedy;
	memcpy(&speedx, &*pkg.data.begin(),2);
	memcpy(&speedy, &*pkg.data.begin()+2,2);
	if(pWheelbase){
		pWheelbase->SetSpeedLocalXY(speedx, speedy);
	}
}

void UiProtocol::RequestSetMotorByIdHandler(const Bluetooth::Package& pkg){
	uint8_t motor_id;
	int16_t speed;
	memcpy(&motor_id, &*pkg.data.begin(),1);
	memcpy(&speed, &*pkg.data.begin()+1,2);

	if(pWheelbase){
		pWheelbase->MotorSetPower(motor_id, speed);
	}
}
