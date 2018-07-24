/*
 * test.cpp
 *
 *  Created on: Jun 14, 2018
 *      Author: dipsy
 */

#include "test.h"

Test::Test():lcd(GetLcdConfig()),writer(GetTypeWriterConfig()){
	pScheduler = new Scheduler(1, 75000*250);
	pProtocol = new Protocol(1,pScheduler,this);
}

Test::~Test(){
	delete pProtocol;
	delete pScheduler;
}

void Test::test1(){
	pProtocol->RequestSetMotor(123);
	pProtocol->AwaitRequestEncoder(&writer);
}

void Test::test2(){

}

St7735r::Config Test::GetLcdConfig(){
	St7735r::Config lcd_config;
	lcd_config.orientation = 0;
	return lcd_config;
}

LcdTypewriter::Config Test::GetTypeWriterConfig(){
	LcdTypewriter::Config writerconfig;
	writerconfig.lcd = &lcd;
	return writerconfig;
}
