/*
 * wheelbase.h
 *
 *  Created on: Jun 15, 2018
 *      Author: dipsy
 */

#ifndef INC_WHEELBASE_H_
#define INC_WHEELBASE_H_

#include <libsc/st7735r.h>
#include <libsc/lcd_typewriter.h>
#include <libsc/dir_motor.h>
#include <libsc/ab_encoder.h>
#include <libsc/servo.h>

#include <string>

#include "protocol.h"
#include "ui_protocol.h"
#include "scheduler.h"

using namespace libsc;

class Protocol;
class UiProtocol;

class Wheelbase {
public:
	Wheelbase();
	~Wheelbase();

	void MotorSetPower(uint8_t id, int16_t speed);
	void ServoSetDegree(uint8_t id, uint16_t degree);
	void UpdateEncoders();
	int32_t EncoderGetCount(uint8_t id);

	void SetSpeedLocalXY(int16_t speedx, int16_t speedy);

	void UpdateEncoderToRemote();

	void Test1();

	void TestOpenMVProtocol();

	bool flag = false;
private:

	Protocol* pProtocol;
	UiProtocol* pUiProtocol;
	Scheduler* pScheduler;
//	St7735r lcd;
//	LcdTypewriter writer;

//	St7735r::Config GetLcdConfig();
//
//	LcdTypewriter::Config GetTypeWriterConfig();

	DirMotor::Config GetMotorConfig(uint8_t id){
		DirMotor::Config config;
		config.id = id;
		return config;
	}

	AbEncoder::Config GetEncoderConfig(uint8_t id){
		AbEncoder::Config config;
		config.id = id;
		return config;
	}

	Servo::Config GetServoConfig(uint8_t id){
		Servo::Config config;
		config.id = id;
		config.period = 20000;
		config.min_pos_width = 1000;
		config.max_pos_width = 2000;
		return config;
	}

	DirMotor motor0, motor1;
	AbEncoder encoder0, encoder1;
	Servo servo;

//	int32_t prev_encoder2_count = 0;
	int32_t encoder_counts[3] = {0,0,0};

	float globalRotation = 0;
	uint16_t globalRotationLapse = 0;
	time_t globalRotationReceivedTime = 0;

	int32_t globalTranslationX = 0, globalTranslationY = 0;
	uint16_t globalTranslationLapse = 0;
	time_t globalTranslationReceivedTime = 0;

	friend Protocol;
	friend UiProtocol;

};



#endif /* INC_WHEELBASE_H_ */
