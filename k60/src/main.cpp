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

int main(void)
{
	System::Init();

	//type your code here
	Led::Config led_config;
    led_config.id = 0;
    Led led0(led_config);
	while (true){
	    led0.Switch();
	    System::DelayMs(250);
	}

	return 0;
}
