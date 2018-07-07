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
	case Bluetooth::PkgType::kResponseEncoderById:
		ResponseEncoderByIdHandler(pkg);
		break;
	case Bluetooth::PkgType::kResponseEncoder:
		ResponseEncoderHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestSetServo:
		RequestSetServoHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestAutoFeedEncoders:
		RequestAutoFeedEncodersHandler(pkg);
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

uint8_t Protocol::ResponseEncoderById(uint8_t id, int32_t count){
	vector<Byte> data(5,0);
	memcpy(&*data.begin(), &id, 1);
	memcpy(&*data.begin()+1, &count, 4);
	return m_bt.QueuePackage({Bluetooth::PkgType::kResponseEncoderById,0,data});
}

uint8_t Protocol::RequestSetServo(uint16_t degree){
	vector<Byte> data(2,0);
	memcpy(&*data.begin(),&degree,2);
	return m_bt.QueuePackage({Bluetooth::PkgType::kRequestSetServo,0,data});
}

uint8_t Protocol::RequestAutoFeedEncoders(uint16_t interval){
	vector<Byte> data(2,0);
	memcpy(&*data.begin(),&interval,2);
	return m_bt.QueuePackage({Bluetooth::PkgType::kRequestAutoFeedEncoders,0,data});
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
		ResponseEncoder(pWheelbase->EncoderGetCount(0));
	} else {
		ResponseEncoder(0);
	}
}

void Protocol::ResponseEncoderHandler(const Bluetooth::Package& pkg){
	waiting_encoder = false;
	memcpy(&encoder_count, &*pkg.data.begin(),4);
	if(pWheelbase){
		pWheelbase->encoder_counts[2] = encoder_count;
	}
	if(test != nullptr){
//		char c[20];
//		sprintf(c,"%d obt en %d", filteredRecievedPackageSumByType[pkg.type], count);
//		test->lcd.SetRegion({0,40,100,20});
//		test->writer.WriteString(c);
		test->flag = true;
	}
}

int32_t Protocol::AwaitRequestEncoder(LcdTypewriter* pwriter){
	RequestEncoder();
	waiting_encoder = true;
	while(waiting_encoder){
		char ch = '0'+waiting_encoder;
		if(pwriter)pwriter->WriteBuffer(&ch,1);
		if(waiting_encoder == false){
			break;
		}
	}
	return encoder_count;
}

void Protocol::RequestSetServoHandler(const Bluetooth::Package& pkg){
	uint16_t degree;
	memcpy(&degree, &*pkg.data.begin(),2);
	if(test != nullptr){
//		char c[20];
//		sprintf(c,"%d req sm %d", filteredRecievedPackageSumByType[pkg.type], speed);
//		test->lcd.SetRegion({0,0,100,20});
//		test->writer.WriteString(c);
	}
	if(pWheelbase){
		pWheelbase->ServoSetDegree(0,degree);
	}
}

void Protocol::ResponseEncoderByIdHandler(const Bluetooth::Package& pkg){
//	uint8_t id;
	int32_t count;
//	memcpy(&id, &*pkg.data.begin(),1);
	memcpy(&count, &*pkg.data.begin()+1,4);
	if(pWheelbase){
		//since it is sent from slave, it is assumed to be id 2 for master
		pWheelbase->encoder_counts[2] = count;
	}
}

void Protocol::RequestAutoFeedEncodersHandler(const Bluetooth::Package& pkg){
	uint16_t interval;
	memcpy(&interval, &*pkg.data.begin(),2);
	if(pWheelbase){
		//clear the previous auto feed job
		pWheelbase->pScheduler->ClearInterval(auto_feed_encoder_job_id);

		//non zero interval reschedule
		if(interval){
			auto_feed_encoder_job_id = pWheelbase->pScheduler->SetInterval([&]{
				int32_t encoder_img = pWheelbase->encoder_counts[0];
				pWheelbase->UpdateEncoders();
				if(encoder_img != pWheelbase->encoder_counts[0]){
					ResponseEncoder(pWheelbase->encoder_counts[0]);
				}
			},interval);
		}
	}
}
