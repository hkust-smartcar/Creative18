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
	if(test){
//		char c[20];
//		sprintf(c,"sum %d %d", m_bt.receive_package_count, m_bt.bytecount);
//		test->lcd.SetRegion({0,100,100,20});
//		test->writer.WriteString(c);
	}
//	if(pkg.type != Bluetooth::PkgType::kACK){
//		if(recievedPackageId[pkg.id%10]){
//			return;
//		}
//		recievedPackageId[pkg.id%10] = true;
//		recievedPackageId[(pkg.id+5)%10] = false;
//	}
//	filteredRecievedPackageSumByType[pkg.type]++;
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

uint8_t Protocol::ResponseEncoder(int32_t count){
	vector<Byte> data(4,0);
	memcpy(&*data.begin(),&count,4);
	return m_bt.QueuePackage({Bluetooth::PkgType::kResponseEncoder,0,data});
}

void Protocol::RequestSetMotorHandler(const Bluetooth::Package& pkg){
	int16_t speed;
	memcpy(&speed, &*pkg.data.begin(),2);
	if(test != nullptr){
//		char c[20];
//		sprintf(c,"%d req sm %d", filteredRecievedPackageSumByType[pkg.type], speed);
//		test->lcd.SetRegion({0,0,100,20});
//		test->writer.WriteString(c);
	}
	if(pWheelbase){
		pWheelbase->MotorSetPower(0,speed);
	}
}

void Protocol::RequestEncoderHandler(const Bluetooth::Package& pkg){
	if(test != nullptr){
//		char c[20];
//		sprintf(c,"%d req en", filteredRecievedPackageSumByType[pkg.type]);
//		test->lcd.SetRegion({0,20,100,20});
//		test->writer.WriteString(c);
	}
	if(pWheelbase){
		ResponseEncoder(encoder_total+=pWheelbase->EncoderGetCount(0));
	} else {
		ResponseEncoder(0);
	}
}

void Protocol::ResponseEncoderHandler(const Bluetooth::Package& pkg){
	waiting_encoder = false;
	memcpy(&encoder_total, &*pkg.data.begin(),4);
	if(test != nullptr){
//		char c[20];
//		sprintf(c,"%d obt en %d", filteredRecievedPackageSumByType[pkg.type], count);
//		test->lcd.SetRegion({0,40,100,20});
//		test->writer.WriteString(c);
		test->flag = true;
	}
}

int32_t Protocol::AwaitRequestEncoder(){
	RequestEncoder();
	waiting_encoder = true;
	while(waiting_encoder){
		if(waiting_encoder == false){
			break;
		}
	}
	return encoder_total;
}
