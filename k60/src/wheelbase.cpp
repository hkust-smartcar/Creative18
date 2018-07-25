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
#if defined(K60_2018_CREATIVE)
encoder0(GetEncoderConfig(0)),
encoder1(GetEncoderConfig(1)),
#else
encoder2(GetEncoderConfig(0)),
#endif
servo(GetServoConfig(0))
//lcd(GetLcdConfig()),
//writer(GetTypeWriterConfig())
{
#if defined(K60_2018_CREATIVE2)
	Gpo::Config gpoConfig;
	for(uint8_t i = 0; i < 6; i++){
		gpoConfig.pin = magnetPins[i];
		pMagnets[i] = new Gpo(gpoConfig);
	}
	pScheduler = new Scheduler(0, 75000*250);
#else
	pScheduler = new Scheduler(0, 75000);
#endif
	pUiProtocol = new UiProtocol(0,pScheduler, this);
	pProtocol = new Protocol(2,pScheduler, this);
	for(uint8_t i = 0; i < MOTOR_CNT; i++){
		pMotors[i] = new DirMotor(GetMotorConfig(i));
	}
}

Wheelbase::~Wheelbase(){
	delete pProtocol;
	delete pUiProtocol;
	delete pScheduler;
	for(uint8_t i = 0; i < MOTOR_CNT; i++){
		delete pMotors[i];
	}
#if not defined(K60_2018_CREATIVE)
	Gpo::Config gpoConfig;
	for(uint8_t i = 0; i < 6; i++){
		gpoConfig.pin = magnetPins[i];
		delete pMagnets[i];
	}
#endif
}

void Wheelbase::MotorSetPower(uint8_t id, int16_t speed){
#if defined(K60_2018_CREATIVE)
	switch(id){
	case 0:
	case 1:
		pMotors[id]->SetClockwise(speed<0);
		pMotors[id]->SetPower(abs(speed));
		break;
	case 2:
		pProtocol->RequestSetMotor(speed);
		break;
	}
#else
	switch(id){
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		pMotors[id]->SetPower(abs(speed));
		pMotors[id]->SetClockwise(speed<0);

	}
#endif
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
#if defined(K60_2018_CREATIVE)
	encoder0.Update();
	encoder_counts[0] += encoder0.GetCount();
	encoder1.Update();
	encoder_counts[1] += encoder1.GetCount();
#else
	encoder2.Update();
	encoder_counts[2] += encoder2.GetCount();
//	encoders.Update();
#endif
//	encoder_counts[2] = pProtocol->AwaitRequestEncoder();
}

int32_t Wheelbase::EncoderGetCount(uint8_t id){
//	Wheelbase::UpdateEncoders();
#if defined(K60_2018_CREATIVE)
	return encoder_counts[id];
#else
	return encoder_counts[id];
#endif
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
		if(System::Time()>nextRender){
			console.Listen();
			console.ListItemValues();
			nextRender = System::Time()+100;
		}
		if(System::Time()%250==0)
		led1.Switch();
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

void Wheelbase::TestUartEncoders(){
#if defined(K60_2018_CREATIVE2)
	pProtocol->RequestAutoFeedEncodersById(0,1);
	pProtocol->RequestAutoFeedEncodersById(1,1);
	pUiProtocol->RequestMove(0,0);
#endif
//	pProtocol->ResponseEncoderById(0,1234);
//	pProtocol->ResponseEncoderById(1,4567);

	Led::Config ledconfid;
	ledconfid.id = 0;
	Led led0(ledconfid);

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

	int32_t motor_powers[8]={};

	int32_t mag = 0;

	DebugConsole console(&joystick,&lcd,&writer,10);
	console.PushItem("mag",&mag,"1","0");
	console.PushItem("encoder0",&encoder_counts[0],float(0.0));
	console.PushItem("encoder1",&encoder_counts[1],float(0.0));
	console.PushItem("encoder2",&encoder_counts[2],float(0.0));
	char c[20];
	for(int i=0;i<8;i++){
		sprintf(c,"motor%d",i);
		console.PushItem(c,&motor_powers[i],float(100));
	}

	console.ListItems();

	while(1){
		console.Listen();
		for(int i=0;i<8;i++){
			MotorSetPower(i,motor_powers[i]);
		}
		for(int i=0;i<6;i++){
			pMagnets[i]->Set(mag&(1<<i));
		}
		UpdateEncoders();
		console.ListItemValues();
		led0.Switch();
		System::DelayMs(10);

	}
}
