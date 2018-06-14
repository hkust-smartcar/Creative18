/*
 * protocol.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#include "protocol.h"

void Protocol::Handler(const Bluetooth::Package& pkg){
	Byte debug[100];
	memset(debug,0,100);
	memcpy(debug,&*pkg.data.begin(),pkg.data.size());
	switch(pkg.type){
	case Bluetooth::PkgType::kRequestSetMotor:
		RequestSetMotorHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestEncoder:
		RequestEncoderHandler(pkg);
		break;
	case Bluetooth::PkgType::kResponseEncoder:
		ResponseEncoderHandler(pkg);
		break;
	}
}

uint8_t Protocol::RequestSetMotor(int16_t speed){
	vector<Byte> data(2,0);
	memcpy(&*data.begin(),&speed,2);
	return m_bt.QueuePackage({Bluetooth::PkgType::kRequestSetMotor,0,data});
}

uint8_t Protocol::RequestEncoder(){
	return m_bt.QueuePackage({Bluetooth::PkgType::kRequestEncoder,0,{}});
}

uint8_t Protocol::ResponseEncoder(uint16_t count){
	vector<Byte> data(2,0);
	memcpy(&*data.begin(),&count,2);
	return m_bt.QueuePackage({Bluetooth::PkgType::kResponseEncoder,0,data});
}

void Protocol::RequestSetMotorHandler(const Bluetooth::Package& pkg){
	int16_t speed;
	memcpy(&speed, &*pkg.data.begin(),2);
	if(test != nullptr){
		char c[20];
		sprintf(c,"req sm %d", speed);
		test->writer.WriteString(c);
	}
}

void Protocol::RequestEncoderHandler(const Bluetooth::Package& pkg){
	if(test != nullptr){
		char c[20];
		sprintf(c,"req en");
		test->writer.WriteString(c);
	}
	ResponseEncoder(7766);
}

void Protocol::ResponseEncoderHandler(const Bluetooth::Package& pkg){
	uint16_t count;
	memcpy(&count, &*pkg.data.begin(),2);
	if(test != nullptr){
		char c[20];
		sprintf(c,"obt en %d", count);
		test->writer.WriteString(c);
	}
}
