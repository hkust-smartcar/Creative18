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
encoder1(GetEncoderConfig(1)),
servo(GetServoConfig(0))
//lcd(GetLcdConfig()),
//writer(GetTypeWriterConfig())
{
	pScheduler = new Scheduler(0, 75000*250);
	pProtocol = new Protocol(pScheduler, this);
	pUiProtocol = new UiProtocol(pScheduler, this);
}

Wheelbase::~Wheelbase(){
	delete pProtocol;
	delete pUiProtocol;
	delete pScheduler;
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

void Wheelbase::ServoSetDegree(uint8_t id, uint16_t degree){
	switch(id){
	case 0:
		servo.SetDegree(degree);
		break;
	case 1:
		pProtocol->RequestSetServo(degree);
		break;
	}
}

void Wheelbase::UpdateEncoders(){
	encoder0.Update();
	encoder_counts[0] += encoder0.GetCount();
	encoder1.Update();
	encoder_counts[1] += encoder1.GetCount();
//	encoder_counts[2] = pProtocol->AwaitRequestEncoder();
}

int32_t Wheelbase::EncoderGetCount(uint8_t id){
//	Wheelbase::UpdateEncoders();
	return encoder_counts[id];
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

void Wheelbase::TestOpenMVProtocol(){
	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);
	led_config.id = 1;
	Led led1(led_config);

	St7735r::Config lcd_config;
	lcd_config.orientation = 0;
	St7735r lcd(lcd_config);
	lcd.Clear();

	LcdTypewriter::Config writerconfig;
	writerconfig.lcd = &lcd;
	LcdTypewriter writer(writerconfig);

	Joystick::Config joystick_config;
	joystick_config.id = 0;
	joystick_config.is_active_low = true;
	Joystick joystick(joystick_config);

	DebugConsole console(&joystick,&lcd,&writer,10);
	console.PushItem("lRotation",&localRotation,0);
	console.PushItem("gRotation",&globalRotation,0);
	console.PushItem("lX",&localTranslationX,0.0);
	console.PushItem("lY",&localTranslationY,0.0);
	console.PushItem("gX",&globalTranslationX,0.0);
	console.PushItem("gY",&globalTranslationY,0.0);

	console.ListItems();

	time_t nextRender = 0;

	while (true){
		if(System::Time()%10==0){
			console.Listen();
			console.ListItemValues();
			if(System::Time()%250==0)
			led0.Switch();
		}
	}
}

void Wheelbase::TestScheduler(){
	pProtocol->RequestAutoFeedEncoders(10);
//	pUiProtocol->RequestMove(0,0);
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
