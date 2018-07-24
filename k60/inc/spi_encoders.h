/*
 * spi_encoder.h
 *
 *  Created on: Jul 22, 2018
 *      Author: dipsy
 */

#ifndef INC_SPI_ENCODERS_H_
#define INC_SPI_ENCODERS_H_


#include <libbase/k60/spi_master.h>
#include <libbase/k60/gpio.h>

using namespace libbase::k60;

#include "mcp23s17.h"

class SpiEncoders{
public:
	SpiEncoders():
	m_spi(GetSpiConfig()),
	m_slaveselect(GetSlaveSelectConfig()),
	m_reset(GetResetConfig()){
		m_reset.Set();
		for(uint8_t i = 0; i < 8; i++){
			pMembers[i] = new Mcp23s17(&m_spi, &m_slaveselect, i);
		}
		m_reset.Reset();
	}

	~SpiEncoders(){
		for(uint8_t i = 0; i < 8; i++){
			delete pMembers[i];
		}
	}

	void Update(){
		for(uint8_t i = 0; i < 8; i++){
			counts[i] = pMembers[i]->ByteRead(Mcp23s17::Registers::kGPIOA);
		}
		m_reset.Set();
		m_reset.Reset();
	}

	int32_t GetCount(uint8_t id){
		return counts[id];
	}

	void ResetCounts(){
		memset(counts,0,sizeof(counts));
	}

private:
	SpiMaster m_spi;
	Gpo m_slaveselect;
	Gpo m_reset;
	Mcp23s17 *pMembers[8];
	int32_t counts[8] = {};

	SpiMaster::Config GetSpiConfig(){
		SpiMaster::Config config;
		SpiMaster::Config::Slave slave;
		slave.is_active_high = true;
#if defined(K60_2018_CREATIVE)
		slave.cs_pin = Pin::Name::kPtb20;
		config.sin_pin = Pin::Name::kPtb23;
		config.sout_pin = Pin::Name::kPtb22;
		config.sck_pin = Pin::Name::kPtb21;
#else
		slave.cs_pin = Pin::Name::kPta14;
		config.sin_pin = Pin::Name::kPtd3;
		config.sout_pin = Pin::Name::kPtd2;
		config.sck_pin = Pin::Name::kPtd1;
#endif
		config.baud_rate_khz = 10000;
		config.slaves[0] = slave;
		return config;
	}

	Gpo::Config GetSlaveSelectConfig(){
		Gpo::Config gpoconfig;
#if defined(K60_2018_CREATIVE)
		gpoconfig.pin = Pin::Name::kPtb19;
#else
		gpoconfig.pin = Pin::Name::kPtd0;
#endif
		return gpoconfig;
	}

	Gpo::Config GetResetConfig(){
		Gpo::Config gpoconfig;
#if defined(K60_2018_CREATIVE)
		gpoconfig.pin = Pin::Name::kPtb16;
#else
		gpoconfig.pin = Pin::Name::kPtb22;
#endif
		return gpoconfig;
	}
};



#endif /* INC_SPI_ENCODERS_H_ */
