/*
 * ui_protocol.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#include "ui_protocol.h"

void UiProtocol::Handler(const Bluetooth::Package& pkg){
//	Byte debug[100];
//	memset(debug,0,100);
//	memcpy(debug,&*pkg.data.begin(),pkg.data.size());
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
	case Bluetooth::PkgType::kRequestEncoderById:
		RequestEncoderByIdHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestEncoders:
		RequestEncodersHandler(pkg);
		break;
	case Bluetooth::PkgType::kFeedGlobalRotation:
		FeedGlobalRotationHandler(pkg);
		break;
	case Bluetooth::PkgType::kFeedLocalRotation:
		FeedLocalRotationHandler(pkg);
		break;
	case Bluetooth::PkgType::kFeedGlobalTranslation:
		FeedGlobalTranslationHandler(pkg);
		break;
	case Bluetooth::PkgType::kFeedLocalTranslation:
		FeedLocalTranslationHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestAutoFeedEncoders:
		RequestAutoFeedEncodersHandler(pkg);
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

uint8_t UiProtocol::ResponseEncoderById(uint8_t id, int32_t count){
	vector<Byte> data(5,0);
	memcpy(&*data.begin(), &id, 1);
	memcpy(&*data.begin()+1, &count, 4);
	return m_bt.QueuePackage({Bluetooth::PkgType::kResponseEncoderById,0,data});
}

uint8_t UiProtocol::ResponseEncoders(int32_t count0, int32_t count1, int32_t count2){
	vector<Byte> data(12,0);
	memcpy(&*data.begin(), &count0, 4);
	memcpy(&*data.begin()+4, &count1, 4);
	memcpy(&*data.begin()+8, &count2, 4);
	return m_bt.QueuePackage({Bluetooth::PkgType::kResponseEncoders,0,data});
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

void UiProtocol::RequestEncoderByIdHandler(const Bluetooth::Package& pkg){
	uint8_t encoder_id;
	memcpy(&encoder_id, &*pkg.data.begin(),1);

	if(pWheelbase){
		ResponseEncoderById(encoder_id, pWheelbase->EncoderGetCount(encoder_id));
	}
}

void UiProtocol::RequestEncodersHandler(const Bluetooth::Package& pkg){
	if(pWheelbase){
		ResponseEncoders(
				pWheelbase->EncoderGetCount(0),
				pWheelbase->EncoderGetCount(1),
				pWheelbase->EncoderGetCount(2)
				);
	}
}

void UiProtocol::FeedGlobalRotationHandler(const Bluetooth::Package& pkg){
	if(pWheelbase){
		memcpy(&pWheelbase->globalRotation, &*pkg.data.begin(), 4);
		memcpy(&pWheelbase->globalRotationLapse, &*pkg.data.begin() + 4, 2);
		pWheelbase->globalRotationReceivedTime = System::Time();
		pWheelbase->globalRotation = pWheelbase->globalRotation*180/3.14159;
	}
}

void UiProtocol::FeedLocalRotationHandler(const Bluetooth::Package& pkg){
	if(pWheelbase){
		memcpy(&pWheelbase->localRotation, &*pkg.data.begin(), 4);
		memcpy(&pWheelbase->localRotationLapse, &*pkg.data.begin() + 4, 2);
		pWheelbase->localRotationReceivedTime = System::Time();
		pWheelbase->localRotation = pWheelbase->localRotation*180/3.14159;
	}
}

void UiProtocol::FeedGlobalTranslationHandler(const Bluetooth::Package& pkg){
	if(pWheelbase){
		memcpy(&pWheelbase->globalTranslationX, &*pkg.data.begin(), 4);
		memcpy(&pWheelbase->globalTranslationY, &*pkg.data.begin() + 4, 4);
		memcpy(&pWheelbase->globalTranslationLapse, &*pkg.data.begin() + 8, 2);
		pWheelbase->globalTranslationReceivedTime = System::Time();
	}
}

void UiProtocol::FeedLocalTranslationHandler(const Bluetooth::Package& pkg){
	if(pWheelbase){
		memcpy(&pWheelbase->localTranslationX, &*pkg.data.begin(), 4);
		memcpy(&pWheelbase->localTranslationY, &*pkg.data.begin() + 4, 4);
		memcpy(&pWheelbase->localTranslationLapse, &*pkg.data.begin() + 8, 2);
		pWheelbase->localTranslationReceivedTime = System::Time();
	}
}

void UiProtocol::RequestAutoFeedEncodersHandler(const Bluetooth::Package& pkg){
	uint16_t interval;
	memcpy(&interval, &*pkg.data.begin(),2);
	if(pWheelbase){
		//clear the previous auto feed job
		pWheelbase->pScheduler->ClearInterval(auto_feed_encoder_job_id);

		//non zero interval reschedule
		if(interval){
			auto_feed_encoder_job_id = pWheelbase->pScheduler->SetInterval([&]{
				pWheelbase->UpdateEncoders();
				for(int i=0; i<3; i++){
					if(encoder_img[i] != pWheelbase->encoder_counts[i]){
						ResponseEncoderById(i,pWheelbase->encoder_counts[i]);
					}
				}
				memcpy(encoder_img,pWheelbase->encoder_counts,12);
			},interval);
		}
	}
}
