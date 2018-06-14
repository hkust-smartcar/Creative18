/*
 * test.cpp
 *
 *  Created on: Jun 14, 2018
 *      Author: dipsy
 */

#include "test.h"

Test::Test():lcd(GetLcdConfig()),writer(GetTypeWriterConfig()){
pProtocol = new Protocol(this);
}

Test::~Test(){
	delete pProtocol;
}

void Test::test1(){
	pProtocol->RequestSetMotor(123);
	pProtocol->RequestEncoder();
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
