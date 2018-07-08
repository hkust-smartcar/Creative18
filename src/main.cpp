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
#include <libutil/pid_controller.h>
#include <libutil/Incremental_Pid_Controller.h>
#include <libutil/positional_pid_controller.h>

#include "kalman_filter.h"

const float pi = 3.14159265358979323846;

namespace libbase {
namespace k60 {

Mcg::Config Mcg::GetMcgConfig() {
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

int32_t encoder_value0 = 0, encoder_value1 = 0, encoder_value2 = 0;
//uint16_t servoAngle = 0;
//int32_t motorSpeed = 0, motorSpeedImg = 0;
//int car = 0;
//bool yo = false;

template<typename T>
inline constexpr T clamp(T l, T v, T u) {
	return (l < v ? (v < u ? v : u) : l);
}

void master() {

	Wheelbase wheelbase;
	wheelbase.MotorSetPower(0, 300);
	wheelbase.MotorSetPower(1, 300);
	wheelbase.MotorSetPower(2, 300);

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

	float power[3] = { 0 };

	float Mea[3] = { 0 };

	float test_ = 0;

	DebugConsole console(&joystick, &lcd, &writer, 10);
//	console.PushItem("servo_angle", &servoAngle, 1);
	console.PushItem("encoder0", &encoder_value0, float(12.0));
	console.PushItem("encoder1", &encoder_value1, float(12.0));
	console.PushItem("encoder2", &encoder_value2, float(12.0));
	console.PushItem("motor0", &power[0], float(1.0));

//	console.PushItem("motor0", &power[0], float(1.0));
//	console.PushItem("motor1", &power[1], float(1.0));
//	console.PushItem("motor2", &power[2], float(1.0));
	console.PushItem("test:", &test_, float(1.0));

	console.ListItems();

	time_t nextRender = 0;

	time_t velocity_timer = 0;
//
//	PositionalPidController<float, float> pid0(
//						wheelbase.motor_target_velocity[0], 5, 0, 0);
	IncrementalPidController<float, float> pid0(
			wheelbase.motor_target_velocity[0], 5, 0, 0);
	IncrementalPidController<float, float> pid1(
			wheelbase.motor_target_velocity[1], 5, 0, 0);
	IncrementalPidController<float, float> pid2(
			wheelbase.motor_target_velocity[2], 5, 0, 0);
//	uint8_t counter = 0;

	float velocity[3] = { 0 };

	pid0.SetOutputBound(-1000, 1000);
	pid1.SetOutputBound(-1000, 1000);
	pid2.SetOutputBound(-1000, 1000);

//	int32_t encoder_value[20][3] = { 0 };
	int32_t encoder_value[3] = { 0 };

	wheelbase.target_x_velocity = 0;
	wheelbase.target_y_velocity = 0;
	wheelbase.target_w_velocity = 0;
	wheelbase.set_motor_target_velocity();

	float kp = 25;
	float ki = 1;
	float kd = 0.01;

	pid0.SetILimit(30);
	pid0.SetSetpoint(wheelbase.motor_target_velocity[0]);
	pid0.SetKp(kp);
	pid0.SetKi(ki);
	pid0.SetKd(kd);

	pid1.SetILimit(30);
	pid1.SetSetpoint(wheelbase.motor_target_velocity[1]);
	pid1.SetKp(kp);
	pid1.SetKi(ki);
	pid1.SetKd(kd);

	pid2.SetILimit(30);
	pid2.SetSetpoint(wheelbase.motor_target_velocity[2]);
	pid2.SetKp(kp);
	pid2.SetKi(ki);
	pid2.SetKd(kd);

	float past_encoder_value[3] = { 0 };

	IncrementalPidController<float, float>* pids[3] = { &pid0, &pid1, &pid2 };

	while (true) {

		console.Listen();

		if (System::Time() > nextRender) {
			led0.Switch();
			console.ListItems();
			nextRender = System::Time() + 1000;
//			wheelbase.plotgraph();

		}
		if (System::Time() > velocity_timer) {

			for (int i = 0; i < 3; i++) {
				wheelbase.UpdateEncoders();

//				wheelbase.set_motor_target_velocity();
				encoder_value[i] = wheelbase.EncoderGetCount(i);
//				pids[i]->SetSetpoint(wheelbase.motor_target_velocity[i]);
//				Mea[i] = (float) (encoder_value[i] - past_encoder_value[i])
//						/ (System::Time() - velocity_timer) * 10;
//				velocity[i] = (float) velocity[i]
//						+ (Mea[i] - velocity[i]) * 0.8;
//				past_encoder_value[i] = encoder_value[i];
//				test_ = pids[i]->Calc(velocity[i]);
//				power[i] = clamp<float>(-1100, power[i] + test_, 1100);
//				wheelbase.MotorSetPower(i, power[i]);
			}
			velocity_timer += 10;
		}

	}
}

void master_feature() {

	Wheelbase wheelbase;

	wheelbase.MotorSetPower(0, 0);
	wheelbase.MotorSetPower(1, 0);
	wheelbase.MotorSetPower(2, 0);

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

	float power[3] = { 0 };

	linalg::vec<float, 3> Mea(0.0f);

	DebugConsole console(&joystick, &lcd, &writer, 10);
	console.PushItem("encoder0", &encoder_value0, float(12.0));
	console.PushItem("encoder1", &encoder_value1, float(12.0));
	console.PushItem("encoder2", &encoder_value2, float(12.0));
	console.PushItem("motor0", &power[0], float(1.0));
	console.PushItem("v0", &wheelbase.m_velocity[0], float(12.0));
	console.PushItem("v1", &wheelbase.m_velocity[1], float(12.0));
	console.PushItem("v2", &wheelbase.m_velocity[2], float(12.0));

	wheelbase.reflash_gamma(0.0);

	console.ListItems();

	time_t nextRender = 0;

	IncrementalPidController<float, float> pid0(
			wheelbase.motor_target_velocity[0], 5, 0, 0);
	IncrementalPidController<float, float> pid1(
			wheelbase.motor_target_velocity[1], 5, 0, 0);
	IncrementalPidController<float, float> pid2(
			wheelbase.motor_target_velocity[2], 5, 0, 0);

	IncrementalPidController<float, float> pidz(0, 0.01, 0, 0);

	float velocity[3] = { 0 };

	pid0.SetOutputBound(-1000, 1000);
	pid1.SetOutputBound(-1000, 1000);
	pid2.SetOutputBound(-1000, 1000);

	float encoder_value[3] = { 0 };

	wheelbase.target_x_velocity = 0;
	wheelbase.target_y_velocity = 0;
	wheelbase.target_w_velocity = 0;

	wheelbase.wheelbase_target_velocity= {wheelbase.target_x_velocity,wheelbase.target_y_velocity,wheelbase.target_w_velocity};

	wheelbase.set_motor_target_velocity();

	float kp = 18;
	float ki = 0.5;
	float kd = 0;

	pid0.SetILimit(40);
	pid0.SetSetpoint(wheelbase.motor_target_velocity[0]);
	pid0.SetKp(kp);
	pid0.SetKi(ki);
	pid0.SetKd(kd);

	pid1.SetILimit(40);
	pid1.SetSetpoint(wheelbase.motor_target_velocity[1]);
	pid1.SetKp(kp);
	pid1.SetKi(ki);
	pid1.SetKd(kd);

	pid2.SetILimit(40);
	pid2.SetSetpoint(wheelbase.motor_target_velocity[2]);
	pid2.SetKp(kp);
	pid2.SetKi(ki);
	pid2.SetKd(kd);

	float past_encoder_value[3] = { 0 };

	IncrementalPidController<float, float>* pids[3] = { &pid0, &pid1, &pid2 };

	int counter = 0;

	time_t velocity_timer = System::Time();
	time_t time = System::TimeIn125us();
	time_t lasttime = System::TimeIn125us();
	float dt = (float) (time - lasttime) / 8000;

	while (true) {

		console.Listen();

		if (System::Time() > velocity_timer) {

			wheelbase.UpdateEncoders();

			time = System::TimeIn125us();

			dt = (float) (time - lasttime) / 8000;

			for (int i = 0; i < 3; i++) {
				encoder_value[i] = (float) wheelbase.EncoderGetCount(i);

				Mea[i] = (float) (encoder_value[i] - past_encoder_value[i]) * 2
						* pi / dt / 1300; //w*r=l=2 pi r d(encoder)/1320
				wheelbase.m_velocity[i] = (float) wheelbase.m_velocity[i] * 0.4
						+ Mea[i] * .6; //sum n=0 to inf w*x*(1-x)^n=w
				past_encoder_value[i] = (float) encoder_value[i];
			}

			wheelbase.reflash_gamma(wheelbase.m_position.z);

			wheelbase.m_position = wheelbase.m_position
					+ dt
							* linalg::mul(wheelbase.get_gamma(),
									wheelbase.m_velocity);

			switch (counter) {
			case 0:
				wheelbase.wheelbase_target_velocity.x = 20;
				wheelbase.wheelbase_target_velocity.y = 0;

				if (wheelbase.m_position.x > 49.5) {
					counter++;
				}
				break;
			case 1:
				wheelbase.wheelbase_target_velocity.x = 0;
				wheelbase.wheelbase_target_velocity.y = 20;

				if (wheelbase.m_position.y > 49.5) {
					counter++;
				}
				break;
			case 2:
				wheelbase.wheelbase_target_velocity.x = -20;
				wheelbase.wheelbase_target_velocity.y = 0;

				if (wheelbase.m_position.x < 0) {
					counter++;
				}
				break;
			case 3:
				wheelbase.wheelbase_target_velocity.x = 0;
				wheelbase.wheelbase_target_velocity.y = -20;

				if (wheelbase.m_position.y < 0) {
					counter = 0;
				}
				break;
			}

//			if(wheelbase.m_position.z<(2*pi))
//			{
				wheelbase.wheelbase_target_velocity.z = 0.5;
//			}
//			else
//			{
//				wheelbase.wheelbase_target_velocity.z = 0;
//
//			}

//			wheelbase.wheelbase_target_velocity.x=0;
//
//			wheelbase.wheelbase_target_velocity.y=0;

			//wheelbase.wheelbase_target_velocity.z=0;


			wheelbase.set_motor_target_velocity();

			for (int i = 0; i < 3; i++) {
				pids[i]->SetSetpoint(wheelbase.motor_target_velocity[i]);
				power[i] = clamp<float>(-1100,
						power[i] + pids[i]->Calc(wheelbase.m_velocity[i]),
						1100);
				wheelbase.MotorSetPower(i, power[i]);
			}

			velocity_timer += 5;
			lasttime = time;
			led0.SetEnable((System::Time() % 250) > 125);
		}

	}
}

void slave() {

	Wheelbase wheelbase;

	Led::Config led_config;
	led_config.id = 0;
	Led led0(led_config);
	led_config.id = 1;
	Led led1(led_config);
	time_t nextRender = 0;
	while (1) {
		if (System::Time() > nextRender) {
			led0.SetEnable(System::Time() % 200);
			nextRender += 5;
		}
	}
}
void test() {

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

	DebugConsole console(&joystick, &lcd, &writer, 10);
	console.PushItem("encoder0", &encoder_value0, float(0.0));
	console.PushItem("encoder1", &encoder_value1, float(0.0));
	console.PushItem("encoder2", &encoder_value2, float(0.0));

	console.ListItems();

	time_t nextRender = 0;

	while (true) {

		console.Listen();

		encoder_value0 = wheelbase.EncoderGetCount(0);
		encoder_value1 = wheelbase.EncoderGetCount(1);
		encoder_value2 = wheelbase.EncoderGetCount(2);
		wheelbase.UpdateEncoders();

		if (System::Time() > nextRender) {
			led0.Switch();
			console.ListItems();
			nextRender = System::Time() + 500;
		}

	}

}

void master_3() {

	Wheelbase wheelbase;

	wheelbase.MotorSetPower(0, 0);
	wheelbase.MotorSetPower(1, 0);
	wheelbase.MotorSetPower(2, 0);

	float power[3] = { 0 };

	linalg::vec<float, 3> Mea(0.0f);

	time_t nextRender = 0;

	IncrementalPidController<float, float> pid0(
			wheelbase.motor_target_velocity[0], 5, 0, 0);
	IncrementalPidController<float, float> pid1(
			wheelbase.motor_target_velocity[1], 5, 0, 0);
	IncrementalPidController<float, float> pid2(
			wheelbase.motor_target_velocity[2], 5, 0, 0);

	IncrementalPidController<float, float> pidz(0, 0.01, 0, 0);

	float velocity[3] = { 0 };

	pid0.SetOutputBound(-1000, 1000);
	pid1.SetOutputBound(-1000, 1000);
	pid2.SetOutputBound(-1000, 1000);

	float encoder_value[3] = { 0 };

	float kp = 22;
	float ki = 0.5;
	float kd = 0;

	pid0.SetILimit(40);
	pid0.SetSetpoint(0);
	pid0.SetKp(kp);
	pid0.SetKi(ki);
	pid0.SetKd(kd);

	pid1.SetILimit(40);
	pid1.SetSetpoint(0);
	pid1.SetKp(kp);
	pid1.SetKi(ki);
	pid1.SetKd(kd);

	pid2.SetILimit(40);
	pid2.SetSetpoint(0);
	pid2.SetKp(kp);
	pid2.SetKi(ki);
	pid2.SetKd(kd);

	linalg::vec<float, 3> wheelbase_target_velocity = { 0, 0, 0 };

	linalg::vec<float, 3> speed = { 0, 0, 0 };

	float past_encoder_value[3] = { 0 };

	IncrementalPidController<float, float>* pids[3] = { &pid0, &pid1, &pid2 };

	int counter = 0;

	time_t velocity_timer = System::Time();
	time_t time = System::TimeIn125us();
	time_t lasttime = System::TimeIn125us();
	float dt = (float) (time - lasttime) / 8000;

	while (true) {

		if (System::Time() > velocity_timer) {

			wheelbase.UpdateEncoders();

			time = System::TimeIn125us();

			dt = (float) (time - lasttime) / 8000;

			for (int i = 0; i < 3; i++) {
				encoder_value[i] = (float) wheelbase.EncoderGetCount(i);

				Mea[i] = (float) (encoder_value[i] - past_encoder_value[i]) * 2
						* pi / dt / 1300; //w*r=l=2 pi r d(encoder)/1320
				speed[i] = (float) speed[i] * 0.4 + Mea[i] * .6; //sum n=0 to inf w*x*(1-x)^n=w
				past_encoder_value[i] = (float) encoder_value[i];
			}

			wheelbase.m_position = wheelbase.m_position
					+ dt
							* linalg::mul(wheelbase.get_gamma(),
									wheelbase.m_velocity);

			wheelbase.wheelbase_target_velocity.z = 0;

			wheelbase.set_motor_target_velocity();

			for (int i = 0; i < 3; i++) {
				pids[i]->SetSetpoint(wheelbase.motor_target_velocity[i]);
				power[i] = clamp<float>(-1100,
						power[i] + pids[i]->Calc(wheelbase.m_velocity[i]),
						1100);
				wheelbase.MotorSetPower(i, power[i]);
			}

			velocity_timer += 5;
			lasttime = time;
		}

	}

}


int main(void) {
	System::Init();
//	test();
//	master();
//	pid_tuning();
//	slave();
//	master_3();
	master_feature();
	return 0;
}
