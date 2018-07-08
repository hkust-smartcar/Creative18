/*
 * wheelbase.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: dipsy
 */

#include "wheelbase.h"
#include "linalg.h"

const float pi = 3.14159265358979323846;

Wheelbase::Wheelbase() :
		motor0(GetMotorConfig(0)), motor1(GetMotorConfig(1)), encoder0(
				GetEncoderConfig(0)), encoder1(GetEncoderConfig(1)), angle_in_deg(
				{ 0.0f, 120.0f, 240.0f }), angle(angle_in_deg[0] / 180 * pi,
				angle_in_deg[1] / 180 * pi, angle_in_deg[2] / 180 * pi), gamma(
				linalg::mat<float, 3, 3>(float(0))), inverse_gamma(0.0f), wheelbase_target_velocity(
				0.0f), motor_target_velocity(0.0f), m_velocity(0, 0, 0), m_position(
				0.0f) {
	pProtocol = new Protocol(this);
	pUiProtocol = new UiProtocol(this);
	reflash_gamma(0);

}
//
//		angle( { angle_in_deg[0] / 360 * 2 * pi,
//				 angle_in_deg[1] / 360 * 2* pi,
//				 angle_in_deg[2] / 360 * 2 * pi })
//	inverse_gamma(linalg::vec<float,3>(0,0,0),linalg::vec<float,3>(0,0,0),linalg::vec<float,3>(0,0,0))

Wheelbase::~Wheelbase() {
	delete pProtocol;
	delete pUiProtocol;
}

void Wheelbase::MotorSetPower(uint8_t id, int16_t speed) {
	switch (id) {
	case 0:
		motor0.SetClockwise(speed < 0);
		motor0.SetPower(abs(speed));
		break;
	case 1:
		motor1.SetClockwise(speed < 0);
		motor1.SetPower(abs(speed));
		break;
	case 2:
		pProtocol->RequestSetMotor(speed);
		break;
	}
}

void Wheelbase::UpdateEncoders() {
	encoder0.Update();
	if ((encoder0.GetCount() < 1000) && (encoder0.GetCount() > -1000)) {
		encoder_counts[0] += encoder0.GetCount();
	}
	encoder1.Update();
	if ((encoder1.GetCount() < 1000) && (encoder1.GetCount() > -1000)) {
		encoder_counts[1] += encoder1.GetCount();
	}
	encoder_counts[2] = pProtocol->AwaitRequestEncoder();
}

int32_t Wheelbase::EncoderGetCount(uint8_t id) {
//Wheelbase::UpdateEncoders();
	return encoder_counts[id];
	switch (id) {
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

void Wheelbase::SetSpeedLocalXY(int16_t speedx, int16_t speedy) {
	//TODO: set the three motor correspondingly
}

void Wheelbase::UpdateEncoderToRemote() {
	int32_t count = EncoderGetCount(0);
	if (count)
		pProtocol->ResponseEncoder(count);
}

void Wheelbase::Test1() {
	pProtocol->RequestSetMotor(123);
	pProtocol->RequestEncoder();
}

void Wheelbase::set_motor_target_velocity() {
	this->motor_target_velocity = linalg::mul(this->inverse_gamma,
			this->wheelbase_target_velocity);
}

void Wheelbase::plotgraph() {
	pUiProtocol->ResponseEncodersImmediate(this->EncoderGetCount(0),
			this->EncoderGetCount(1), this->EncoderGetCount(2));
}

//void Wheelbase::reflash_gamma(float theta) {
//	gamma = linalg::mat<float, 3, 3>(linalg::cos(angle), linalg::sin(angle),
//			linalg::vec<float, 3>((float)1 /3 /19.3564064606));
//			//linalg::vec<float, 3>((float)1 / 19.3564064606*3));
//
//
////	gamma=linalg::map<float,3,3>(gamma,[](float x)->float{return x*3.5f;});
//	gamma=gamma*3.5f/3.0f;
//	gamma = linalg::transpose(gamma);
//	gamma = linalg::mul(
//			linalg::mat<float, 3, 3>(
//					linalg::vec<float, 3>(std::cos(theta), std::sin(theta),
//							0.0f),
//					linalg::vec<float, 3>(-std::sin(theta), std::cos(theta),
//							0.0f), linalg::vec<float, 3>(0.0f, 0.0f, 1.0f)),
//			gamma);
//
//	inverse_gamma = linalg::inverse(gamma);
//}

void Wheelbase::reflash_gamma(float theta) {
	inverse_gamma = linalg::mat<float, 3, 3>(linalg::cos(angle+theta), linalg::sin(angle+theta),
			linalg::vec<float, 3>((float)23.5f));
	inverse_gamma=inverse_gamma/3.5f;

	gamma = linalg::inverse(inverse_gamma);
}


void Wheelbase::calc_with_encoder_velocity(float w1, float w2, float w3) {

}

