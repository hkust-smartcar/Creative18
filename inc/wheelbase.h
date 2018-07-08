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
#include <libsc/ab_encoder.h>

#include "protocol.h"
#include "ui_protocol.h"
#include "linalg.h"


using namespace libsc;
using namespace linalg;

class Protocol;
class UiProtocol;

//motor radius~3.5cm
//encoder~1320->1 round
class Wheelbase {
public:
	Wheelbase();
	~Wheelbase();

	linalg::vec<float,3> m_position;
	linalg::vec<float,3> m_velocity;

//	struct State{
//		float x;
//		float y;
//		float theta;
////		float x_dot;
////		float y_dot;
////		float theta_dot;
////		State():x(0),y(0),theta(0),x_dot(0),y_dot(0),theta_dot(0){};
//		State():x(0),y(0),theta(0){};
//
//	};

	void reflash_gamma(float theta);


	void MotorSetPower(uint8_t id, int16_t speed);
	void UpdateEncoders();
	int32_t EncoderGetCount(uint8_t id);

	void SetSpeedLocalXY(int16_t speedx, int16_t speedy);

	void UpdateEncoderToRemote();

	//int16_t motor_target_velocity[3];

	float target_x_velocity;
	float target_y_velocity;
	float target_w_velocity;

	linalg::vec<float,3> wheelbase_target_velocity;
	linalg::vec<float,3> motor_target_velocity;

	linalg::mat<float,3,3> get_gamma(void){
		return this->gamma;
	}

	linalg::mat<float,3,3> get_inverse_gamma(void){
		return this->inverse_gamma;
	}

	void set_motor_target_velocity();
	void plotgraph();

	void calc_with_encoder_velocity(float, float, float);


	void Test1();

	bool flag = false;
private:
	const float m_wheelbase_radius=20.0f;

	Protocol* pProtocol;
	UiProtocol* pUiProtocol;

	DirMotor::Config GetMotorConfig(uint8_t id){
		DirMotor::Config config;
		config.id = id;
		return config;
	}

	AbEncoder::Config GetEncoderConfig(uint8_t id){
		DirEncoder::Config config;
		config.id = id;
		return config;
	}

	DirMotor motor0, motor1;
	AbEncoder encoder0, encoder1;


	float angle_in_deg[3];
	//float gamma[3][3];

	linalg::vec<float,3> angle;
	linalg::mat<float,3,3> gamma;
	linalg::mat<float,3,3> inverse_gamma;



//	State m_state;

	int32_t encoder_counts[3] = {0,0,0};

	friend Protocol;
	friend UiProtocol;
//	friend Kalman_Filter;

};



#endif /* INC_WHEELBASE_H_ */
