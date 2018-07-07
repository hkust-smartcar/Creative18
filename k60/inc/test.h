/*
 * test.h
 *
 *  Created on: Jun 14, 2018
 *      Author: dipsy
 */

#ifndef INC_TEST_H_
#define INC_TEST_H_

#include <libsc/st7735r.h>
#include <libsc/lcd_typewriter.h>
#include "protocol.h"
#include "scheduler.h"

using namespace libsc;

class Protocol;

class Test {
public:
	Test();
	~Test();
	void test1();
	void test2();

	bool flag = false;
private:
	Protocol* pProtocol;
	St7735r lcd;
	LcdTypewriter writer;
	Scheduler* pScheduler;

	St7735r::Config GetLcdConfig();

	LcdTypewriter::Config GetTypeWriterConfig();

	friend Protocol;
};




#endif /* INC_TEST_H_ */
