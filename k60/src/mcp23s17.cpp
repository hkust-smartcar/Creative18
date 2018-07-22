/*
 * mcp23s17.cpp
 *
 *  Created on: Jul 18, 2018
 *      Author: dipsy
 */

#include "mcp23s17.h"

Mcp23s17::Mcp23s17(SpiMaster* pSpi, Gpo* slaveSelect, uint8_t address, uint8_t slave_id):pSpi(pSpi),pGpo(slaveSelect), address(address),slave_id(slave_id){
//		ByteWrite(Registers::kIOCON, OpCodes::kADDR_ENABLE);
	}

void Mcp23s17::ByteWrite(uint8_t reg, uint8_t value){
	pGpo->Reset();
	pSpi->ExchangeData(slave_id,OpCodes::kOPCODEW | (address << 1));
	pSpi->ExchangeData(slave_id,reg);
	pSpi->ExchangeData(slave_id,value);
	pGpo->Set();
}

uint8_t Mcp23s17::ByteRead(uint8_t reg){
	uint8_t value = 0;                        // Initialize a variable to hold the read values to be returned
	pGpo->Reset();                 // Take slave-select low
	pSpi->ExchangeData(slave_id,OpCodes::kOPCODER | (address << 1));  // Send the MCP23S17 opcode, chip address, and read bit
	pSpi->ExchangeData(slave_id,reg);                        // Send the register we want to read
	value = pSpi->ExchangeData(slave_id,0x00);               // Send any byte, the function will return the read value
	pGpo->Set();                // Take slave-select high
	return value;                             // Return the constructed word, the format is 0x(register value)
}

uint16_t Mcp23s17::WordRead(){
	uint16_t value = 0;                        // Initialize a variable to hold the read values to be returned
	pGpo->Reset();                 // Take slave-select low
	pSpi->ExchangeData(slave_id,OpCodes::kOPCODER | (address << 1));  // Send the MCP23S17 opcode, chip address, and read bit
	pSpi->ExchangeData(slave_id,Registers::kGPIOA);                        // Send the register we want to read
	value = pSpi->ExchangeData(slave_id,Registers::kGPIOB);               // Send any byte, the function will return the read value
	value |= pSpi->ExchangeData(slave_id,0x00) << 8;
	pGpo->Set();                // Take slave-select high
	return value;                             // Return the constructed word, the format is 0x(register value)
}
