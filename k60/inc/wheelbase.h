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
	int32_t EncoderGetCount(uint8_t id);

	void SetSpeedLocalXY(int16_t speedx, int16_t speedy);

	void UpdateEncoderToRemote();

	void Test1();

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

	int32_t prev_encoder2_count = 0;

	friend Protocol;
	friend UiProtocol;

};



#endif /* INC_WHEELBASE_H_ */
