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
#include "libbase/k60/gpio.h"
#include "libbase/k60/spi_master.h"

#include "debug_console.h"
#include "test.h"
#include "wheelbase.h"
#include "scheduler.h"
#include "mcp23s17.h"

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
using namespace libsc::k60;
using namespace libbase::k60;

int32_t encoder_value0 = 0,encoder_value1 = 0,encoder_value2 = 0;
uint16_t servoAngle0 = 0, servoAngleImg0 = -1, servoAngle1 = 0, servoAngleImg1 = -1;
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
	console.PushItem("servo_angle0",&servoAngle0,1);
	console.PushItem("servo_angle1",&servoAngle1,1);
	console.PushItem("encoder0",&encoder_value0,float(0.0));
	console.PushItem("encoder1",&encoder_value1,float(0.0));
	console.PushItem("encoder2",&encoder_value2,float(0.0));
	console.PushItem("motorSpeed",&motorSpeed,10);

	console.ListItems();

	time_t nextRender = 0;

	while (true){

		console.Listen();


	    encoder_value0 =wheelbase.EncoderGetCount(0);
	    encoder_value1 =wheelbase.EncoderGetCount(1);
		encoder_value2 =wheelbase.EncoderGetCount(2);
		wheelbase.UpdateEncoders();

	    if(System::Time()>nextRender){
	    	led0.Switch();
	    	console.ListItems();
	    	nextRender = System::Time()+500;
		}


	    if(motorSpeedImg!=motorSpeed){
	    	motorSpeedImg = motorSpeed;
			wheelbase.MotorSetPower(0,motorSpeed);
			wheelbase.MotorSetPower(1,motorSpeed);
			wheelbase.MotorSetPower(2,motorSpeed);
	    }

	    if(servoAngleImg0 != servoAngle0){
	    	servoAngleImg0 = servoAngle0;
			wheelbase.ServoSetDegree(0,servoAngle0);
	    }

	    if(servoAngleImg1 != servoAngle1){
			servoAngleImg1 = servoAngle1;
			wheelbase.ServoSetDegree(1,servoAngle1);
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
			wheelbase.UpdateEncoders();
			wheelbase.UpdateEncoderToRemote();
			led0.Switch();
			nextRender+=250;
		}
	}
}

void test(){
	Wheelbase wheelbase;

	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);
	led_config.id = 1;
	Led led1(led_config);
	time_t nextRender = 0;
	while(1){
		if(System::Time()>nextRender){
			wheelbase.TestOpenMVProtocol();

			led0.Switch();
			nextRender+=250;
		}
	}
//	Test t;
//	t.test1();
//
//	Led::Config led_config;
//	led_config.id = 0;
//	Led led0(led_config);
//	led_config.id = 1;
//	Led led1(led_config);
//
//	while (true){
//		while(!t.flag){
//			led0.Switch();
//			System::DelayMs(250);
//		}
//		while(t.flag){
//			led0.Switch();
//			System::DelayMs(500);
//		}
//		if(t.flag){
//			led1.SetEnable(t.flag);
//		}
//	}
}

void testScheduler(){
	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);
	led_config.id = 1;
	Led led1(led_config);
	Scheduler scheduler(0,75000*100);
	int x = scheduler.SetInterval([&]{
		led0.Switch();
	},250);
	scheduler.ClearInterval(x);
	while(1);
}

void testOpenMV(){
	Wheelbase wb;
	wb.TestOpenMVProtocol();
}

void testMCP(){
	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);

	SpiMaster::Config config;
	SpiMaster::Config::Slave slave;
	slave.cs_pin = Pin::Name::kPtb20;
	slave.is_active_high = true;
	config.baud_rate_khz = 10000;
	config.sin_pin = Pin::Name::kPtb23;
	config.sout_pin = Pin::Name::kPtb22;
	config.sck_pin = Pin::Name::kPtb21;
	config.slaves[0] = slave;

	SpiMaster spi(config);

	St7735r::Config lcd_config;
	St7735r lcd(lcd_config);
	lcd.Clear();

	LcdTypewriter::Config writerconfig;
	writerconfig.lcd = &lcd;
	LcdTypewriter writer(writerconfig);

	Gpo::Config gpoconfig;
	gpoconfig.pin = Pin::Name::kPtb19;
	Gpo ss = Gpo(gpoconfig);

	Mcp23s17 mcp0(&spi,&ss,0);
	Mcp23s17 mcp1(&spi,&ss,7);
	char c[20];
	while(1){
		lcd.SetRegion({0,0,100,15});
		sprintf(c,"%d %d",mcp0.WordRead(), mcp1.WordRead());
		writer.WriteString(c);


		ss.Reset();
		uint16_t data1 = spi.ExchangeData(0,0b01000001);
		uint16_t data2 = spi.ExchangeData(0,0x12);
		uint16_t data3 = spi.ExchangeData(0,0x13);
		uint16_t data4 = spi.ExchangeData(0,0x00);
		ss.Set();
		sprintf(c,"%d %d %d %d",data1,data2,data3,data4);
		lcd.SetRegion({0,40,128,20});
		writer.WriteString(c);

		bool state[16];
		uint8_t mask = 1;
		for(int i=0; i<8; i++){
			state[i] = data3 & mask;
			mask<<=1;
		}
		mask = 1;
		for(int i=8; i<16; i++){
			state[i] = data4 & mask;
			mask<<=1;
		}
		lcd.SetRegion({0,60,100,15});
		sprintf(c,"%d%d%d%d %d%d%d%d",state[0],state[1],state[2],state[3],state[4],state[5],state[6],state[7]);
		writer.WriteString(c);

		lcd.SetRegion({0,80,100,15});
		sprintf(c,"%d%d%d%d %d%d%d%d",state[8],state[9],state[10],state[11],state[12],state[13],state[14],state[15]);
		writer.WriteString(c);


		led0.Switch();
		System::DelayMs(100);
	}
}

void testSpiEncoders(){
	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);

	St7735r::Config lcd_config;
	St7735r lcd(lcd_config);
	lcd.Clear();

	LcdTypewriter::Config writerconfig;
	writerconfig.lcd = &lcd;
	LcdTypewriter writer(writerconfig);

	SpiEncoders encoders;
	int32_t counts[8] = {};
	char c[20];
	while(1){
		encoders.Update();
		for(int i=0; i<8; i++){
			counts[i]=encoders.GetCount(i);
			sprintf(c,"%d",counts[i]);
			int y = i*20;
			lcd.SetRegion({0,y,128,20});
			writer.WriteString(c);
			System::DelayMs(1);
		}
		led0.Switch();
		System::DelayMs(10);
	}

}

void testUartEncoder(){
	Wheelbase wb;
	wb.TestUartEncoders();
}

int main(void)
{
	System::Init();

	testUartEncoder();

//	testMCP();
//	testSpiEncoders();

//	testOpenMV();
//	test();
//	master();
//	slave();
//	test();

//	testScheduler();
//	testUiProtocolWithScheduler();

	return 0;
}
