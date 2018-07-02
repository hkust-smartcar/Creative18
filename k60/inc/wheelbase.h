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
#include <libsc/dir_encoder.h>

#include <string>

#include "protocol.h"
#include "ui_protocol.h"

using namespace libsc;

class Protocol;
class UiProtocol;

class Wheelbase {
public:
	Wheelbase();
	~Wheelbase();

	void MotorSetPower(uint8_t id, int16_t speed);
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

	DirEncoder::Config GetEncoderConfig(uint8_t id){
		DirEncoder::Config config;
		config.id = id;
		return config;
	}

	DirMotor motor0, motor1;
	DirEncoder encoder0, encoder1;

//	int32_t prev_encoder2_count = 0;
	int32_t encoder_counts[3] = {0,0,0};

	float globalRotation = 0;
	uint8_t globalRotationLapse = 0;
	time_t globalRotationReceivedTime = 0;

	int32_t globalTranslationX = 0, globalTranslationY = 0;
	uint8_t globalTranslationLapse = 0;
	time_t globalTranslationReceivedTime = 0;

	friend Protocol;
	friend UiProtocol;

};



#endif /* INC_WHEELBASE_H_ */
