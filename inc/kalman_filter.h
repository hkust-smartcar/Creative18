///*
// * Kalman_Filter.h
// *
// *  Created on: 26 Jun 2018
// *      Author: Job
// */
//
//#ifndef INC_KALMAN_FILTER_H_
//#define INC_KALMAN_FILTER_H_
//
//#include <array>
//#include <vector>
//#include "wheelbase.h"
//#include "martix.h"
//
//class Wheelbase;
//
//class Kalman_Filter{
//public:
//	Kalman_Filter(Wheelbase* wb);
//
//	void calc_encoder(float,float,float);
//private:
//	//std::array<float>
//
//	Wheelbase* pWheelbase;
//
////	Wheelbase::State m_state;
//
//
//
//	//initial covariance matrix
//	//this refer to the error in the inital state
//	std::vector<std::vector<float>> m_P;
//
//	//measurement covariance martix of the encoder readings
//	std::vector<std::vector<float>> m_Q;
//
//	//Kalman Gain
//	//This determant how much we trust the readings
//	std::vector<std::vector<float>> m_K;
//
//
//	friend Wheelbase;
////	constexpr static float Gamma_Local[3][3]={{std::cos()},{},{}};
//};
//
//
//
//
//
//
//#endif /* INC_KALMAN_FILTER_H_ */
