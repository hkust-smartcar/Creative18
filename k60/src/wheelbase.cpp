/*
 * wheelbase.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: dipsy
 */

#include "wheelbase.h"

Wheelbase::Wheelbase():
//lcd(GetLcdConfig()),
//writer(GetTypeWriterConfig()),
motor0(GetMotorConfig(0)),
motor1(GetMotorConfig(1)),
encoder0(GetEncoderConfig(0)),
encoder1(GetEncoderConfig(1)){
	pProtocol = new Protocol(this);
	pUiProtocol = new UiProtocol(this);
}

Wheelbase::~Wheelbase(){
	delete pProtocol;
	delete pUiProtocol;
}

void Wheelbase::MotorSetPower(uint8_t id, int16_t speed){
	switch(id){
	case 0:
		motor0.SetClockwise(speed<0);
		motor0.SetPower(abs(speed));
		break;
	case 1:
		motor1.SetClockwise(speed<0);
		motor1.SetPower(abs(speed));
		break;
	case 2:
		pProtocol->RequestSetMotor(speed);
		break;
	}
}

void Wheelbase::UpdateEncoders(){
	encoder0.Update();
	encoder_counts[0] += encoder0.GetCount();
	encoder1.Update();
	encoder_counts[1] += encoder1.GetCount();
	encoder_counts[2] = pProtocol->AwaitRequestEncoder();
}

int32_t Wheelbase::EncoderGetCount(uint8_t id){
//	Wheelbase::UpdateEncoders();
	return encoder_counts[id];
	switch(id){
	case 0:
		encoder0.Update();
		return encoder0.GetCount();
	case 1:
		encoder1.Update();
		return encoder1.GetCount();
	case 2:
//		pProtocol->RequestEncoder();
//		int32_t count = pProtocol->GetEncoderTotolCount() - prev_encoder2_count;
//		prev_encoder2_count = pProtocol->GetEncoderTotolCount();
//		int32_t newCount = pProtocol->AwaitRequestEncoder();
//		int32_t count = newCount - prev_encoder2_count;
//		prev_encoder2_count = newCount;
		return pProtocol->AwaitRequestEncoder();
	}
	return 0;
}

void Wheelbase::SetSpeedLocalXY(int16_t speedx, int16_t speedy){
	//TODO: set the three motor correspondingly
}

void Wheelbase::UpdateEncoderToRemote(){
	int32_t count = EncoderGetCount(0);
	if(count)pProtocol->ResponseEncoder(count);
}

void Wheelbase::Test1(){
	pProtocol->RequestSetMotor(123);
	pProtocol->RequestEncoder();
}
//St7735r::Config Wheelbase::GetLcdConfig(){
//	St7735r::Config lcd_config;
//	lcd_config.orientation = 0;
//	return lcd_config;
//}
//
//LcdTypewriter::Config Wheelbase::GetTypeWriterConfig(){
//	LcdTypewriter::Config writerconfig;
//	writerconfig.lcd = &lcd;
//	return writerconfig;
//}
