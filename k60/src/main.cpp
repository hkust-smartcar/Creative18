/*
 * main.cpp
 *
 * Author: 
 * Copyright (c) 2014-2015 HKUST SmartCar Team
 * Refer to LICENSE for details
 */

#include <cassert>
#include <cstring>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <libsc/led.h>

#include "debug_console.h"
#include "test.h"
#include "wheelbase.h"

namespace libbase
{
	namespace k60
	{

		Mcg::Config Mcg::GetMcgConfig()
		{
			Mcg::Config config;
			config.external_oscillator_khz = 50000;
			config.core_clock_khz = 150000;
			return config;
		}

	}
}

using libsc::System;
using namespace libsc;
using namespace libbase::k60;

int32_t encoder_value0 = 0,encoder_value1 = 0,encoder_value2 = 0;
uint16_t servoAngle = 0;
int32_t motorSpeed = 0, motorSpeedImg = 0;
int car = 0;
bool yo = false;

template<typename T>
inline constexpr T clamp(T l, T v, T u){return (l<v?(v<u?v:u):l);}


void master(){

	Wheelbase wheelbase;

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
	console.PushItem("servo_angle",&servoAngle,1);
	console.PushItem("encoder0",&encoder_value0,float(0.0));
	console.PushItem("encoder1",&encoder_value1,float(0.0));
	console.PushItem("encoder2",&encoder_value2,float(0.0));
	console.PushItem("motorSpeed",&motorSpeed,10);

	console.ListItems();

	time_t nextRender = 0;

	while (true){

		console.Listen();


	    encoder_value0 += clamp<int>(-100,wheelbase.EncoderGetCount(0),100);
	    encoder_value1 += clamp<int>(-100,wheelbase.EncoderGetCount(1),100);

	    if(System::Time()>nextRender){
	    	led0.Switch();
	    	console.ListItems();
	    	nextRender = System::Time()+500;
	    	encoder_value2 += clamp<int>(-100,wheelbase.EncoderGetCount(2),100);
		}


	    if(motorSpeedImg!=motorSpeed){
	    	motorSpeedImg = motorSpeed;
			wheelbase.MotorSetPower(0,motorSpeed);
			wheelbase.MotorSetPower(1,motorSpeed);
			wheelbase.MotorSetPower(2,motorSpeed);
	    }
	}
}

void slave(){

	Wheelbase wheelbase;

	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);
	led_config.id = 1;
	Led led1(led_config);
	time_t nextRender = 0;
	while(1){
		if(System::Time()>nextRender){

			led0.Switch();
			nextRender+=250;
		}
		wheelbase.UpdateEncoderToRemote();
	}
}

void test(){
	Test t;
	t.test1();

	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);
	led_config.id = 1;
	Led led1(led_config);

	while (true){
		while(!t.flag){
			led0.Switch();
			System::DelayMs(250);
		}
		while(t.flag){
			led0.Switch();
			System::DelayMs(500);
		}
		if(t.flag){
			led1.SetEnable(t.flag);
		}
	}
}

int main(void)
{
	System::Init();


//	test();
	master();
//	slave();


	return 0;
}
