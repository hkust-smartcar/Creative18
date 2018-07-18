/*
 * mcp23s17.h
 *
 *  Created on: Jul 18, 2018
 *      Author: dipsy
 */

#pragma once

#include "libbase/k60/pin.h"
#include "libbase/k60/gpio.h"
#include "libbase/k60/spi_master.h"

using libbase::k60::SpiMaster;
using libbase::k60::Gpo;
using libbase::k60::Pin;

namespace libsc{
namespace k60{
class Mcp23s17{
public:

	struct Registers{
		const static uint8_t kIODIRA =   (0x00),      // MCP23x17 I/O Direction Register
		kIODIRB =   (0x01),      // 1 = Input (default), 0 = Output

		kIPOLA =    (0x02),      // MCP23x17 Input Polarity Register
		kIPOLB =    (0x03),      // 0 = Normal (default)(low reads as 0), 1 = Inverted (low reads as 1)

		kGPINTENA =  (0x04),      // MCP23x17 Interrupt on Change Pin Assignements
		kGPINTENB =  (0x05),      // 0 = No Interrupt on Change (default), 1 = Interrupt on Change

		kDEFVALA =   (0x06),      // MCP23x17 Default Compare Register for Interrupt on Change
		kDEFVALB =   (0x07),      // Opposite of what is here will trigger an interrupt (default = 0)

		kINTCONA =   (0x08),      // MCP23x17 Interrupt on Change Control Register
		kINTCONB =  (0x09),      // 1 = pin is compared to DEFVAL, 0 = pin is compared to previous state (default)

		kIOCON =     (0x0A),      // MCP23x17 Configuration Register
		//                   (0x0B)      //     Also Configuration Register

		kGPPUA =     (0x0C),      // MCP23x17 Weak Pull-Up Resistor Register
		kGPPUB =     (0x0D),      // INPUT ONLY: 0 = No Internal 100k Pull-Up (default) 1 = Internal 100k Pull-Up

		kINTFA =     (0x0E),      // MCP23x17 Interrupt Flag Register
		kINTFB =     (0x0F),      // READ ONLY: 1 = This Pin Triggered the Interrupt

		kINTCAPA =   (0x10),      // MCP23x17 Interrupt Captured Value for Port Register
		kINTCAPB =   (0x11),      // READ ONLY: State of the Pin at the Time the Interrupt Occurred

		kGPIOA =     (0x12),      // MCP23x17 GPIO Port Register
		kGPIOB =     (0x13),      // Value on the Port - Writing Sets Bits in the Output Latch

		kOLATA =     (0x14),      // MCP23x17 Output Latch Register
		kOLATB =     (0x15);      // 1 = Latch High, 0 = Latch Low (default) Reading Returns Latch State, Not Port Value!
	};

	struct OpCodes{
		const static uint8_t kOPCODEW=       (0b01000000),  // Opcode for MCP23S17 with LSB (bit0) set to write (0), address OR'd in later, bits 1-3
		kOPCODER=       (0b01000001),  // Opcode for MCP23S17 with LSB (bit0) set to read (1), address OR'd in later, bits 1-3
		kADDR_ENABLE=   (0b00001000);  // Configuration register for MCP23S17, the only thing we change is enabling hardware addressing
	};

	Mcp23s17(SpiMaster* pSpi, Gpo* slaveSelect, uint8_t address, uint8_t slave_id = 0):pSpi(pSpi),pGpo(slaveSelect), address(address),slave_id(slave_id){
//		ByteWrite(Registers::kIOCON, OpCodes::kADDR_ENABLE);
	}

	void ByteWrite(uint8_t reg, uint8_t value){
		pGpo->Reset();
		pSpi->ExchangeData(slave_id,OpCodes::kOPCODEW | (address << 1));
		pSpi->ExchangeData(slave_id,reg);
		pSpi->ExchangeData(slave_id,value);
		pGpo->Set();
	}

	uint8_t ByteRead(uint8_t reg){
		uint8_t value = 0;                        // Initialize a variable to hold the read values to be returned
		pGpo->Reset();                 // Take slave-select low
		pSpi->ExchangeData(slave_id,OpCodes::kOPCODER | (address << 1));  // Send the MCP23S17 opcode, chip address, and read bit
		pSpi->ExchangeData(slave_id,reg);                        // Send the register we want to read
		value = pSpi->ExchangeData(slave_id,0x00);               // Send any byte, the function will return the read value
		pGpo->Set();                // Take slave-select high
		return value;                             // Return the constructed word, the format is 0x(register value)
	}

	uint16_t WordRead(){
		uint16_t value = 0;                        // Initialize a variable to hold the read values to be returned
		pGpo->Reset();                 // Take slave-select low
		pSpi->ExchangeData(slave_id,OpCodes::kOPCODER | (address << 1));  // Send the MCP23S17 opcode, chip address, and read bit
		pSpi->ExchangeData(slave_id,Registers::kGPIOA);                        // Send the register we want to read
		value = pSpi->ExchangeData(slave_id,Registers::kGPIOB);               // Send any byte, the function will return the read value
		value |= pSpi->ExchangeData(slave_id,0x00) << 8;
		pGpo->Set();                // Take slave-select high
		return value;                             // Return the constructed word, the format is 0x(register value)
	}

private:
	SpiMaster* pSpi;
	Gpo* pGpo;
	uint8_t slave_id;
	uint8_t address;
};
}
}

